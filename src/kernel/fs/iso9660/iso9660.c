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

#include <errno.h>
#include <fs.h>
#include <kernel.h>
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
	char	name[];
} __attribute__ ((packed));

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

static struct sb_ops iso9660_sb_ops;
static struct inode_ops iso9660_inode_ops;
static struct file_ops iso9660_file_ops;

static struct inode *iso9660_inode_alloc(struct superblock *sp)
{
	struct inode *ip;

	(void) sp;

	if (!(ip = kcalloc(1, sizeof(struct inode))))
		return NULL;

	ip->op = &iso9660_inode_ops;
	ip->fop = &iso9660_file_ops;

	return ip;
}

static struct dirent *iso9660_lookup(struct dirent *dp, const char *name)
{
	struct dirent *dep;

	list_for_each(dep, &dp->ip->del, l)
		if (strcmp(dep->name, name) == 0)
			return dep;

#if 0
	if (!(ip = inode_alloc(dp->ip->sp)))
		goto err;

	ip->inum = addr;
	if (dev_dep->flags & ISO9660_DF_DIR)
		ip->mode |= IM_DIR;
	else
		ip->mode |= IM_REG;

	ip->links = 1;

	/* TODO Read from disk */
	ip->atime = 0;
	ip->ctime = 0;
	ip->mtime = 0;

	ip->size = dev_dep->size;

	/* XXX A very dirty hack */
	dep->ip->op->readdir(dp);

	list_for_each(dep, &dp->ip->del, l) {
		if (strcmp(dep->name, name) == 0)
			return dep;
#endif
#if 0
	struct inode *ip = NULL;
	struct dirent *dep;
	struct iso9660_dirent dev_dep;
	u32 i;

	for (i = ((struct iso9660_sb *) dp->ip->sp->device)->root.size; i;
			i -= dp->ip->sp->block_size) {
		/* XXX TEMP XXX */ atapi_read(dp->ip->sp->dev.minor, &dev_dep,
				((struct iso9660_sb *)
				dp->ip->sp->device)->root.addr,
				2048);
	}

	if (!(ip = inode_alloc(dp->ip->sp)))
		goto err;
	/* TODO Fill in more */
	ip->size = dev_dep.size; /* XXX Right? */

	if (!(dep = dirent_alloc(dp, name)))
		goto err;
	dep->ip = ip;

	return dep;

err:
	if (ip)
		inode_put(ip);
#endif
	return NULL;
}

static int iso9660_readdir(struct dirent *dp)
{
	struct dirent *dep;
	struct iso9660_dirent *dev_dp, *dev_dep;
	char buf[2048], name[NAME_MAX + 1], *sep;
	off_t addr, i;

	dev_dp = (struct iso9660_dirent *) &((struct iso9660_sb *) dp->ip->sp->device)->root;

	addr = dev_dp->addr;

	atapi_read(dp->ip->sp->dev.minor, &buf, addr, 2048);

	for (i = 254; i < 2048; i += dev_dep->length) {
		dev_dep = (struct iso9660_dirent *) buf + i;

		if (!dev_dep->length)
			break;

		memcpy(name, dev_dep->name, dev_dep->name_len);
		name[dev_dep->name_len] = '\0';
		sep = strchr(name, ';');
		*sep = '\0';

		stolower(name);

		if (!(dep = dirent_alloc(dp, name)))
			return -ENOMEM;
	}

	return 0;
}

static struct inode *iso9660_read_sb(struct superblock *sp)
{
	struct inode *ip;
	struct iso9660_sb *dev_sp;
	int i;

	if (!(dev_sp = kmalloc(sizeof(struct iso9660_sb))))
		return NULL;

	for (i = 16; i < 32; i++) {
		/* XXX TEMP XXX */ atapi_read(sp->dev.minor, dev_sp, i, 2048);

		if (strncmp(dev_sp->signature, "CD001", 5) != 0 ||
				dev_sp->version != 1)
			return NULL;

		if (dev_sp->type == 1)
			break;
	}

	strncpy(sp->name, dev_sp->vol_ident, NAME_MAX);
	strtrm(sp->name);

	sp->blocks = dev_sp->blocks;
	sp->block_size = dev_sp->block_size;

	sp->device = dev_sp;

	if (!(ip = inode_alloc(sp)))
		return NULL;

	ip->inum = i;

	sp->root = ip;

	sp->op = &iso9660_sb_ops;

#if 0
	kprintf("# of blocks: %d   block size: %d", sp->blocks, sp->block_size);
	kprintf(" (%d MB)\n", (sp->blocks * sp->block_size) / 1024);
#endif

	return ip;
}

static struct sb_ops iso9660_sb_ops = {
	.inode_alloc	= &iso9660_inode_alloc,
	.inode_dealloc	= NULL,
	.inode_write	= NULL,
	.inode_delete	= NULL
};

static struct inode_ops iso9660_inode_ops = {
	.create		= NULL,
	.link		= NULL,
	.symlink	= NULL,
	.rmlink		= NULL,
	.mkdir		= NULL,
	.rmdir		= NULL,
	.mknod		= NULL,
	.move		= NULL,
	.lookup		= &iso9660_lookup,
	.readdir	= &iso9660_readdir
};

static struct file_ops iso9660_file_ops = {
	NULL
};

static struct fs_driver iso9660_driver = {
	.name		= devname,

	.read_sb	= &iso9660_read_sb
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
