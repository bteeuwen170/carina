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

#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "devfs";

static struct file_ops devfs_file_ops;

static int devfs_sb_get(struct superblock *sp)
{
	strcpy(sp->name, "dev");

	return inode_get(sp, 0, &sp->root);
}

static int devfs_sb_put(struct superblock *sp)
{
	(void) sp;

	return 0;
}

static int devfs_alloc(struct inode *ip)
{
	struct device *devp;

	if (!ip->inum) {
		ip->mode = I_DIR;
		ip->op = &devfs_file_ops;

		return 0;
	}

	list_for_each(devp, &devices, l) {
		if (devp->flags & D_CONTROLLER)
			continue;

		if ((ino_t) devp->dev == ip->inum) {
			ip->mode = I_DEV;
			ip->rdev = devp->dev;
			ip->op = devp->op;

			return 0;
		}
	}

	return -ENOENT;
}

static int devfs_lookup(struct inode *dp, const char *name, struct dirent **dep)
{
	struct dirent *cdep;
	struct device *devp;
	char nbuf[NAME_MAX + 1];

	list_for_each(devp, &devices, l) {
		if (devp->flags & D_CONTROLLER)
			continue;

		sprintf(nbuf, "%s%d", dev_names[MAJOR(devp->dev)],
				MINOR(devp->dev));

		if (strcmp(nbuf, name) != 0)
			continue;

		if (!(cdep = kmalloc(sizeof(struct dirent))))
			return -ENOMEM;

		cdep->inum = (ino_t) devp->dev;
		strcpy(cdep->name, nbuf);

		cdep->sp = dp->sp;
		cdep->pdep = NULL;

		*dep = cdep;

		return 0;
	}

	return -ENOENT;
}

static int devfs_readdir(struct file *fp, char *_name)
{
	struct device *devp;
	int i = 2;

	if (fp->off == 0) {
		_name[0] = '.';
		_name[1] = '\0';
	} else if (fp->off == 1) {
		_name[0] = '.';
		_name[1] = '.';
		_name[2] = '\0';
	} else {
		list_for_each(devp, &devices, l) {
			if (devp->flags & D_CONTROLLER)
				continue;

			if (i++ == fp->off) {
				sprintf(_name, "%s%d",
						dev_names[MAJOR(devp->dev)],
						MINOR(devp->dev));

				fp->off++;

				return 0;
			}
		}

		return -EFAULT;
	}

	fp->off++;

	return 0;
}

static struct fs_ops devfs_fs_ops = {
	.sb_get		= &devfs_sb_get,
	.sb_put		= &devfs_sb_put,

	.alloc		= &devfs_alloc,
	.lookup		= &devfs_lookup,

	.mkreg		= NULL,
	.mkdir		= NULL,
	.mkdev		= NULL,
	.mklnk		= NULL,
	.rmlnk		= NULL,
	.link		= NULL,
	.move		= NULL
};

static struct file_ops devfs_file_ops = {
	.readdir	= &devfs_readdir
};

static struct fs_driver devfs_driver = {
	.name	= "devfs",
	.flags	= M_RO,

	.op	= &devfs_fs_ops
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
