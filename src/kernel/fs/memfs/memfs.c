/*
 *
 * Elarix
 * src/kernel/fs/memfs/memfs.c
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
#include <limits.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "memfs";

static struct file_ops memfs_file_ops;

static int memfs_sb_get(struct superblock *sp)
{
	int res;

	strcpy(sp->name, "mem");

	if ((res = inode_get(sp, 0, &sp->root)) < 0)
		return res;

	sp->root->mode = I_DIR;

	return 0;
}

static int memfs_sb_put(struct superblock *sp)
{
	(void) sp;

	return 0;
}

static int memfs_alloc(struct inode *ip)
{
	ip->op = &memfs_file_ops;

	return 0;
}

static int memfs_lookup(struct inode *dp, const char *name, struct dirent **dep)
{
	(void) dp, (void) name, (void) dep;

	return -ENOENT;
}

static int memfs_mkdir(struct inode *dp, struct dirent *dep, mode_t mode)
{
	struct inode *ip;
	int res;

	dep->inum = ++dp->sp->blocks;

	if ((res = inode_get(dp->sp, dep->inum, &ip)) < 0)
		return res;

	ip->mode = mode;

	list_add(&dp->del, &dep->l);

	return 0;
}

static int memfs_readdir(struct file *fp, char *_name)
{
	struct dirent *cdep;
	int i = 2;

	if (fp->off == 0) {
		_name[0] = '.';
		_name[1] = '\0';
	} else if (fp->off == 1) {
		_name[0] = '.';
		_name[1] = '.';
		_name[2] = '\0';
	} else {
		list_for_each(cdep, &fp->ip->del, l) {
			if (i++ != fp->off)
				continue;

			strcpy(_name, cdep->name);
			fp->off++;

			return 0;
		}

		return -EFAULT;
	}

	fp->off++;

	return 0;
}

static struct fs_ops memfs_fs_ops = {
	.sb_get		= &memfs_sb_get,
	.sb_put		= &memfs_sb_put,

	.alloc		= &memfs_alloc,
	.lookup		= &memfs_lookup,

	/* TODO */
	.mkreg		= NULL,
	.mkdir		= &memfs_mkdir,
	.mkdev		= NULL,
	.mklnk		= NULL,
	.rmlnk		= NULL,
	.link		= NULL,
	.move		= NULL
};

static struct file_ops memfs_file_ops = {
	/* TODO */
	.read		= NULL,
	.write		= NULL,
	.readdir	= &memfs_readdir,
	.ioctl		= NULL
};

static struct fs_driver memfs_driver = {
	.name	= devname,
	.flags	= M_KEEP,

	.op	= &memfs_fs_ops
};

int memfs_init(void)
{
	return fs_reg(&memfs_driver);
}

void memfs_exit(void)
{
	/* TODO */

	fs_unreg(&memfs_driver);
}
