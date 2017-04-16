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

#include <stdlib.h>

static const char devname[] = "ramfs";

struct iso9660_sb {
	u8	type;
	char	signature[5];
	u8	version;
#if 1
	u8	data[2041];
#else
	u8	reserved0;
	u8	unused0[64];
	u8	reserved1[8];
	/* TODO */
#endif
} __attribute__ ((packed));

struct iso9660_date {
	char	year[4];
	char	month[2];
	char	day[2];
	char	hour[2];
	char	minute[2];
	char	second[2];
	char	centisecond[2];
	i8	timezone;
};

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
		/* iso9660_read(sp, i, &sb); */

		if (strncmp(sb.signature, "CD001", 5) != 0 || sb.version != 1)
			return NULL;

		if (sb.type == 1)
			break;
	}

	kprintf("FOUND SB!\n");
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
