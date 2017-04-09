/*
 *
 * Elarix
 * src/kernel/fs/devfs/devfs.c
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

static const char devname[] = "devfs";

static int devfs_mknod(struct inode *dp, struct dirent *dep,
		mode_t mode, dev_t dev)
{
	(void) dp, (void) dep, (void) mode, (void) dev;

	return 0;
}

static struct superblock *devfs_read_sb(struct superblock *sp)
{
	struct inode *ip;
	struct dirent *dep;

	sp->flags = SF_KEEP;

	sp->op = &devfs_sb_ops;

	ip = devfs_inode_alloc(sp, IM_DIR | 0755, (dev_t) { 0, 0 });
	if (!ip)
		return NULL;

	dep = dirent_alloc_root(ip);
	sp->root = dep;

	return sp;
}

static const struct sb_ops devfs_sb_ops = { NULL };

static const struct inode_ops devfs_inode_ops = {
	.mknod = &devfs_mknod
};

static struct fs_driver devfs_driver = {
	.name		= devname,

	.read_sb	= &devfs_read_sb
};

int devfs_init(void)
{
	return fs_reg(&devfs_driver);
}

void devfs_exit(void)
{
	/* TODO */

	fs_unreg(&devfs_driver);
}
