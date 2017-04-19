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

#include <fs.h>
#include <kernel.h>
#include <module.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "iso9660";

struct iso9660_time {
	char	year[4];
	char	month[2];
	char	day[2];
	char	hour[2];
	char	minute[2];
	char	second[2];
	char	centisecond[2];
	i8	timezone;
} __attribute__ ((packed));

struct iso9660_dirent {
	u8	length;
	u8	length_ext;

	u32	addr;
	u8	unused0[4];

	u32	size;
	u8	unused1[4];

	struct iso9660_time mtime;

	u8	flags;

	u8	unit_size;
	u8	unit_gap;

	u16	disk;
	u8	unused2[2];

	u8	name_len;
	char	name[];
} __attribute__ ((packed));

struct iso9660_sb {
	u8	type;
	char	signature[5];
	u8	version;
	u8	reserved0;
	u8	unused0[32];

	char	vol_ident[32];
	u8	reserved1[8];

	u32	blocks;
	u8	unused1[4];
	u8	reserved2[32];

	u16	disks;
	u8	unused2[2];
	u16	disk;
	u8	unused3[2];

	u16	block_size;
	u8	unused4[24];

	struct iso9660_dirent root;

	u8	unused6[1858];
} __attribute__ ((packed));

static struct sb_ops iso9660_sb_ops;
static struct inode_ops iso9660_inode_ops;
/*static struct file_ops iso9660_file_ops;
static struct file_ops iso9660_dir_ops; */

/* static struct inode *iso9660_inode_alloc(struct superblock *sp)
{
	struct inode *ip;

	if (!(ip = inode_alloc(sp)))
		return NULL;

	return ip;
} */

static struct dirent *iso9660_lookup(struct dirent *dp, const char *name)
{
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

	return NULL;
}

static struct inode *iso9660_read_sb(struct superblock *sp)
{
#if 1
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

	sp->blocks = dev_sp->blocks;
	sp->block_size = dev_sp->block_size;

	sp->op = &iso9660_sb_ops;

	if (!(ip = inode_alloc(sp)))
		return NULL;
	return NULL;

	sp->root = ip;

	kprintf("name: %s\n", sp->name);
	kprintf("# of blocks: %d   block size: %d", sp->blocks, sp->block_size);
	kprintf(" (%d MB)\n", (sp->blocks * sp->block_size) / 1024);

	return ip;
#else
	struct inode *ip;
	struct dirent *dep;

	sp->op = &iso9660_sb_ops;

	if (!(ip = iso9660_inode_alloc(sp, IM_DIR | 0755, (dev_t) { 0, 0 })))
		return NULL;
	dep = dirent_alloc_root(ip);
	sp->root = dep;

	if (!(dep = dirent_alloc(sp->root, ".")))
		return NULL;
	dep->ip = ip;

	if (!(dep = dirent_alloc(sp->root, "..")))
		return NULL;
	dep->ip = ip;

	return sp;
#endif
}

static struct sb_ops iso9660_sb_ops = { NULL };

static struct inode_ops iso9660_inode_ops = {
	.create		= NULL,
	.link		= NULL,
	.symlink	= NULL,
	.rmlink		= NULL,
	.mkdir		= NULL,
	.rmdir		= NULL,
	.mknod		= NULL,
	.move		= NULL,
	.lookup		= &iso9660_lookup
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
