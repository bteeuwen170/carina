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

#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <module.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static const char devname[] = "iso9660";

#define ISO9660_DF_DIR	0x02

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
	struct iso9660_sb *dev_sp;
	int res, i;

	if (!(dev_sp = kmalloc(sizeof(struct iso9660_sb))))
		return -ENOMEM;

	for (i = 16; i < 32; i++) {
		/* XXX TEMP XXX */ atapi_read(MINOR(sp->dev), dev_sp, i, 2048);

		if (strncmp(dev_sp->signature, "CD001", 5) != 0 ||
				dev_sp->version != 1)
			return -EINVAL;

		if (dev_sp->type == 1)
			break;
	}

	strncpy(sp->name, dev_sp->vol_ident, NAME_MAX);
	strtrm(sp->name);

	sp->blocks = dev_sp->blocks;
	sp->block_size = dev_sp->block_size;

	if ((res = inode_get(sp, ((struct iso9660_dirent *)
			dev_sp->root)->addr, &sp->root)) < 0)
		goto err;

	sp->root->inum = i * sp->block_size + 156;

	sp->root->block = ((struct iso9660_dirent *) dev_sp->root)->addr;
	sp->root->size = ((struct iso9660_dirent *) dev_sp->root)->size;

	return 0;

err:
	kfree(dev_sp);

	return res;
}

static int iso9660_alloc(struct inode *ip)
{
	struct iso9660_dirent *ddep;
	char *buf;

	/* XXX TEMP */
	ip->size = 2048;

	/* FIXME How do we clean this crap up? */
	if (!(buf = kmalloc(ip->size)))
		return -ENOMEM;

	/* XXX TEMP XXX */ atapi_read(MINOR(ip->sp->dev), buf, (off_t) ip->inum / ip->sp->block_size, ip->size);

	ddep = (struct iso9660_dirent *)
			(buf + ip->inum % ip->sp->block_size);

	ip->block = ddep->addr;
	ip->size = ddep->size;

	/* XXX TEMP XXX */
	ip->mode = I_DIR;
	ip->op = &iso9660_file_ops;
	/* TODO */
	return 0;
}

static int iso9660_lookup(struct inode *dp, const char *name,
		struct dirent **dep)
{
	struct dirent *cdep;
	struct iso9660_dirent *ddep;
	char *buf, nbuf[NAME_MAX + 1];
	off_t p;

	if (!(buf = kmalloc(dp->size)))
		return -ENOMEM;

	/* XXX TEMP XXX */ atapi_read(MINOR(dp->sp->dev), buf, (off_t) dp->block, dp->size);

	for (p = 0; p < dp->size; p += ddep->length) {
		ddep = (struct iso9660_dirent *) (buf + p);

		if (!ddep->length)
			break;

		memcpy(nbuf, ddep->name, ddep->name_len);
		nbuf[ddep->name_len] = '\0';
		*strchr(nbuf, ';') = '\0';

		if (strcmp(nbuf, name) != 0)
			continue;

		/* TODO put_block here */

		if (!(cdep = kmalloc(sizeof(struct dirent))))
			return -ENOMEM;

		cdep->inum = dp->block * dp->sp->block_size + p;
		strcpy(cdep->name, nbuf);

		cdep->sp = dp->sp;
		cdep->pdep = NULL;

		*dep = cdep;

		return 0;
	}

	return -ENOENT;
}

static int iso9660_readdir(struct file *fp, char *_name)
{
	struct iso9660_dirent *ddep;
	char *buf;
	off_t p, i;

	if (!(buf = kmalloc(fp->dp->size)))
		return -ENOMEM;

	/* XXX TEMP XXX */ atapi_read(MINOR(fp->dp->sp->dev), buf, (off_t) fp->dp->block, fp->dp->size);

	for (p = 0, i = 0; p < fp->dp->size; p += ddep->length, i++) {
		ddep = (struct iso9660_dirent *) (buf + p);

		if (!ddep->length)
			break;
		else if (i != fp->off)
			continue;

		/* TODO put_block here */

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

		return 0;
	}

	return -EFAULT;
}

static struct fs_ops iso9660_fs_ops = {
	.sb_get		= &iso9660_sb_get,
	.sb_put		= NULL,

	.alloc		= &iso9660_alloc,
	.lookup		= &iso9660_lookup
};

static struct file_ops iso9660_file_ops = {
	.read		= NULL,
	.write		= NULL,
	.readdir	= &iso9660_readdir,
	.ioctl		= NULL
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
