/*
 *
 * Elarix
 * src/kernel/fs/iso9660/iso9660.c
 *
 * Copyright (C) 2016 - 2017 Bastiaan Teeuwen <bastiaan@mkcl.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#include <block.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <module.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static const char devname[] = "iso9660";

#define ISO9660_DF_DIR	0b10

struct iso9660_time {
	u8	year;
	u8	month;
	u8	day;
	u8	hour;
	u8	minute;
	u8	second;
	i8	timezone;
} __attribute__ ((packed));

struct iso9660_dirent {
	u8	length;
	u8	unused0;

	u32	addr;
	u8	unused1[4];
	u32	size;
	u8	unused2[4];

	struct iso9660_time mtime;

	u8	flags;
	u8	unused3[2];

	u16	disk;
	u8	unused4[2];

	u8	name_len;
	char	name[32];
} __attribute__ ((packed));

#ifdef CONFIG_ROCKRIDGE
struct iso9660_rockridge_dirent {
	u8	mtime[7];
	u8	atime[7];
	u8	ctime[7];
	u8	btime[7];

	mode_t	mode;
	uid_t	uid;
	gid_t	gid;
	dev_t	rdev;
} __attribute__ ((packed));
#endif

struct iso9660_sb {
	u8	type;
	char	signature[5];
	u8	version;
	u8	unused0[33];

	char	vol_ident[32];
	u8	unused1[8];

	u32	blocks;
	u8	unused2[36];

	u16	disks;
	u8	unused3[2];
	u16	disk;
	u8	unused4[2];

	u16	block_size;
	u8	unused5[26];

	char	root[34];

	u8	unused6[1858];
} __attribute__ ((packed));

static struct file_ops iso9660_file_ops;

static int iso9660_sb_get(struct superblock *sp)
{
	struct block *bp = NULL;
	struct iso9660_sb *dev_sp;
	struct iso9660_dirent *dev_dep;
	int res = -EINVAL, i;

	for (i = 16; i < 32; i++) {
		if ((res = block_get(sp->dev, i, &bp)) < 0)
			goto err;
		dev_sp = (struct iso9660_sb *) &bp->buffer;

		if (strncmp(dev_sp->signature, "CD001", 5) != 0 ||
				dev_sp->version != 1)
			goto err;

		if (dev_sp->type == 1)
			goto found;
	}

	goto err;

found:
	dev_dep = (struct iso9660_dirent *) dev_sp->root;

	strncpy(sp->name, dev_sp->vol_ident, NAME_MAX);
	strtrm(sp->name);

	sp->blocks = dev_sp->blocks;
	sp->block_size = dev_sp->block_size;

	if ((res = inode_get(sp, i * sp->block_size + 156, &sp->root)) < 0)
		goto err;

	sp->root->block = dev_dep->addr;
	sp->root->size = dev_dep->size;

	return 0;

err:
	if (bp)
		block_put(bp);

	return res;
}

static int iso9660_alloc(struct inode *ip)
{
	struct block *bp;
	struct iso9660_dirent *ddep;
	int res;

	if ((res = block_get(ip->sp->dev, (off_t) ip->inum / ip->sp->block_size,
			&bp)) < 0)
		return res;
	ddep = (struct iso9660_dirent *)
			(bp->buffer + ip->inum % ip->sp->block_size);

	if (ddep->flags & ISO9660_DF_DIR)
		ip->mode = I_DIR;
	else
		ip->mode = I_REG;

	ip->block = ddep->addr;
	ip->size = ddep->size;

	ip->op = &iso9660_file_ops;

	block_put(bp);

	return 0;
}

static int iso9660_lookup(struct inode *dp, const char *name,
		struct dirent **dep)
{
	struct block *bp;
	struct dirent *cdep = NULL;
	struct iso9660_dirent *ddep;
	char buf[NAME_MAX + 1], *nbuf;
	size_t boff = 0, doff;
	int res;

	do {
		if ((res = block_get(dp->sp->dev, dp->block + boff, &bp)) < 0)
			return res;

		for (doff = 0; doff < ((boff < dp->size / dp->sp->block_size) ?
				dp->sp->block_size :
				dp->size % dp->sp->block_size);
				doff += ddep->length) {
			ddep = (struct iso9660_dirent *) (bp->buffer + doff);

			if (!ddep->length)
				break;

			memcpy(buf, ddep->name, ddep->name_len);
			buf[ddep->name_len] = '\0';

			if ((nbuf = strrchr(buf, ';'))) {
				*--nbuf = '\0';
				if (*--nbuf == '.')
					*nbuf = '\0';
			}

			if (strcmp(buf, name) != 0)
				continue;

			/* TODO put_block here */

			if (!(cdep = kmalloc(sizeof(struct dirent)))) {
				res = -ENOMEM;
				goto err;
			}

			cdep->inum = dp->block * dp->sp->block_size + doff;
			strcpy(cdep->name, buf);

			cdep->sp = dp->sp;
			cdep->pdep = NULL;

			*dep = cdep;

			block_put(bp);

			return 0;
		}

		block_put(bp);
	} while (boff++ < dp->size / dp->sp->block_size);

	res = -ENOENT;

err:
	if (cdep)
		kfree(cdep);
	block_put(bp);

	return res;
}

static int iso9660_read(struct file *fp, char *buf, off_t off, size_t n)
{
	struct block *bp;
	size_t boff = 0;
	int res;
	(void) off; /* XXX Will off ever be used? */

	if (n > fp->ip->size)
		return -EINVAL;

	do {
		if ((res = block_get(fp->ip->sp->dev, fp->ip->block + boff,
				&bp)) < 0)
			return res;

		memcpy(buf + boff * fp->ip->sp->block_size, bp->buffer,
				(boff < n / fp->ip->sp->block_size) ?
				fp->ip->sp->block_size :
				n % fp->ip->sp->block_size);

		block_put(bp);
	} while (boff++ < n / fp->ip->sp->block_size);

	return fp->ip->size;
}

static int iso9660_readdir(struct file *fp, char *_name)
{
	struct block *bp;
	struct iso9660_dirent *ddep;
	char *buf;
	size_t boff = 0, doff;
	off_t i;
	int res;

	do {
		if ((res = block_get(fp->ip->sp->dev, fp->ip->block + boff,
				&bp)) < 0)
			return res;

		for (doff = 0, i = 0; doff < ((boff < fp->ip->size /
				fp->ip->sp->block_size) ?
				fp->ip->sp->block_size :
				fp->ip->size % fp->ip->sp->block_size);
				doff += ddep->length, i++) {
			ddep = (struct iso9660_dirent *) (bp->buffer + doff);

			if (!ddep->length)
				break;
			else if (i != fp->off)
				continue;

			if (ddep->name[0] == 0) {
				_name[0] = '.';
				_name[1] = '\0';
			} else if (ddep->name[0] == 1) {
				_name[0] = _name[1] = '.';
				_name[2] = '\0';
			} else {
				memcpy(_name, ddep->name, ddep->name_len);
				_name[ddep->name_len] = '\0';

				if ((buf = strrchr(_name, ';'))) {
					*--buf = '\0';
					if (*--buf == '.')
						*buf = '\0';
				}
			}

			fp->off++;

			block_put(bp);

			return 0;
		}

		block_put(bp);
	} while (boff++ < fp->ip->size / fp->ip->sp->block_size);

	return -EFAULT;
}

static struct fs_ops iso9660_fs_ops = {
	.sb_get		= &iso9660_sb_get,
	.sb_put		= NULL,

	.alloc		= &iso9660_alloc,
	.lookup		= &iso9660_lookup
};

static struct file_ops iso9660_file_ops = {
	.read		= &iso9660_read,
	.readdir	= &iso9660_readdir
};

static struct fs_driver iso9660_driver = {
	.name	= devname,
	.flags	= M_RO,

	.op	= &iso9660_fs_ops
};

int iso9660_init(void)
{
	return fs_reg(&iso9660_driver);
}

void iso9660_exit(void)
{
	/* TODO */

	fs_unreg(&iso9660_driver);
}

MODULE(iso9660, &iso9660_init, &iso9660_exit);
