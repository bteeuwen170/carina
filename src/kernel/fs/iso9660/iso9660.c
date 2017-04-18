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

	char	sys_ident[32];
	char	vol_ident[32];
	u8	reserved1[8];

	u32	blocks;
	u8	unused0[4];
	u8	reserved2[32];

	u16	disks;
	u8	unused1[2];
	u16	disk;
	u8	unused2[2];

	u16	block_size;
	u8	unused3[24];

	struct iso9660_dirent root;

	u8	unused6[1858];
} __attribute__ ((packed));

static struct inode *iso9660_inode_alloc(struct superblock *sp)
{
	struct inode *ip;

	if (!(ip = kmalloc(sizeof(struct inode))))
		return NULL;

	return ip;
}

static struct inode *iso9660_read_sb(struct superblock *sp)
{
#if 1
	struct iso9660_sb sb;
	int i;

	for (i = 16; i < 32; i++) {
		int ret = atapi_read(sp->dev.minor, &sb, i, 2048);

		if (strncmp(sb.signature, "CD001", 5) != 0 || sb.version != 1)
			return NULL;

		if (sb.type == 1)
			break;
	}

	kprintf("sys: %s\n", sb.sys_ident);
	kprintf("vol: %s\n", sb.vol_ident);
	kprintf("# of blocks: %d   block size: %d", sb.blocks, sb.block_size);
	kprintf(" (%d MB)\n", (sb.blocks * sb.block_size) / 1024);

	return NULL;
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
