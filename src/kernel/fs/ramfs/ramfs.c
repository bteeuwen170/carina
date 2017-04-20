/*
 *
 * Elarix
 * src/kernel/fs/ramfs/ramfs.c
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
#include <limits.h>
#include <kernel.h>
#include <module.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "ramfs";

static struct sb_ops ramfs_sb_ops;
static struct inode_ops ramfs_inode_ops;
static struct file_ops ramfs_file_ops;

static int ramfs_mknod(struct inode *dp, struct dirent *dep,
		mode_t mode, dev_t dev)
{
	(void) dp, (void) dep, (void) mode, (void) dev;

	return 0;
}

static int ramfs_create(struct inode *dp, struct dirent *dep, mode_t mode)
{
	return ramfs_mknod(dp, dep, mode | FM_REG, (dev_t) { 0, 0 });
}

static int ramfs_link(struct inode *dp, struct dirent *dep, struct dirent *name)
{
	(void) dp, (void) dep, (void) name;

	return 0;
}

static int ramfs_symlink(struct inode *dp, struct dirent *dep, const char *name)
{
	(void) dp, (void) dep, (void) name;

	return 0;
}

static int ramfs_rmlink(struct inode *dp, struct dirent *dep)
{
	(void) dp, (void) dep;

	return 0;
}

static int ramfs_mkdir(struct inode *dp, struct dirent *dep, mode_t mode)
{
	return ramfs_mknod(dp, dep, mode | FM_DIR, (dev_t) { 0, 0 });
}

static int ramfs_rmdir(struct inode *dp, struct dirent *dep)
{
	(void) dp, (void) dep;

	return 0;
}

static int ramfs_move(struct inode *odp, struct dirent *odep,
		struct inode *dp, struct dirent *dep)
{
	(void) odp, (void) odep, (void) dp, (void) dep;

	return 0;
}

static struct dirent *ramfs_lookup(struct dirent *dp, const char *name)
{
	struct dirent *dep;

	list_for_each(dep, &dp->ip->del, l)
		if (strcmp(dep->name, name) == 0)
			return dep;

	return NULL;
}

/* static struct inode *ramfs_inode_alloc(struct superblock *sp,
		mode_t mode, dev_t dev)
{
	struct inode *ip;
	(void) dev;

	if (!(ip = inode_alloc(sp)))
		return NULL;

	ip->mode = mode;

	switch (mode & IM_FTM) {
	case IM_DIR:
		ip->op = &ramfs_inode_ops;
		ip->fop = &ramfs_dir_ops;
		break;
	case IM_REG:
		ip->op = &ramfs_inode_ops;
		ip->fop = &ramfs_file_ops;
		break;
	default:
		[>TODO<]
		break;
	}

	return ip;
} */

static struct inode *ramfs_read_sb(struct superblock *sp)
{
	struct inode *ip;

	sp->flags = SF_KEEP;

	sp->op = &ramfs_sb_ops;

	/* if (!(ip = ramfs_inode_alloc(sp, IM_DIR | 0755, (dev_t) { 0, 0 })))
		return NULL; */
	if (!(ip = inode_alloc(sp)))
		return NULL;

	ip->op = &ramfs_inode_ops;
	ip->fop = &ramfs_file_ops;

	return ip;
}

static struct sb_ops ramfs_sb_ops = { NULL };

static struct inode_ops ramfs_inode_ops = {
	.create		= &ramfs_create,
	.link		= &ramfs_link,
	.symlink	= &ramfs_symlink,
	.rmlink		= &ramfs_rmlink,
	.mkdir		= &ramfs_mkdir,
	.rmdir		= &ramfs_rmdir,
	.mknod		= &ramfs_mknod,
	.move		= &ramfs_move,
	.lookup		= &ramfs_lookup,
	.readdir	= NULL
};

static struct file_ops ramfs_file_ops = { NULL };

static struct fs_driver ramfs_driver = {
	.name		= devname,

	.read_sb	= &ramfs_read_sb
};

int ramfs_init(void)
{
	return fs_reg(&ramfs_driver);
}

void ramfs_exit(void)
{
	/* TODO */

	fs_unreg(&ramfs_driver);
}

MODULE(ramfs, &ramfs_init, &ramfs_exit);
