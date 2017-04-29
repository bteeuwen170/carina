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

/* TODO Split this file into dev.c and devfs.c */

#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "dev";

static const char *major_names[] = {
	NULL,
	"zero", "mem", "con", "kbd", "mce", "hdd", "odd", "snd", "rtc",
	[63] = "etc"
};

static u32 minor_last[MAJOR_MAX] = { 0 };

static LIST_HEAD(drivers);
static LIST_HEAD(devices);

static struct file_ops devfs_file_ops;

int driver_reg(struct driver *drip)
{
	struct driver *cdrip;

	list_for_each(cdrip, &drivers, l)
		if (strcmp(cdrip->name, drip->name) == 0)
			return -EEXIST;

	list_add(&drivers, &drip->l);

	return 0;
}

void driver_unreg(struct driver *drip)
{
	struct device *devp;

	if (!drip)
		return;

	list_for_each(devp, &devices, l)
		if (devp->drip == drip)
			device_unreg(devp);

	list_rm(&drip->l);
	kfree(drip);
}

int device_reg(u32 major, struct driver *drip, struct device **devp)
{
	struct device *cdevp;

	if (!major | !drip)
		return -EINVAL;

	if (!(cdevp = kmalloc(sizeof(struct device))))
		return -ENOMEM;

	list_init(&cdevp->l);
	list_add(&devices, &cdevp->l);

	cdevp->device = NULL;
	cdevp->dev = DEV(major, minor_last[major]++);
	cdevp->drip = drip;

	if (devp)
		*devp = cdevp;

	return 0;
}

void device_unreg(struct device *devp)
{
	if (!devp)
		return;

	devp->drip->fini(devp);

	list_rm(&devp->l);
	kfree(devp);
}

struct device *device_get(dev_t dev)
{
	struct device *devp;

	list_for_each(devp, &devices, l)
		if (devp->dev == dev)
			return devp;

	return NULL;
}

dev_t device_getbyname(const char *name)
{
	char *mn;
	unsigned int i;

	for (i = 0; i < sizeof(major_names) / sizeof(major_names[0]); i++)
		if (major_names[i] && (mn = strstr(name, major_names[i])))
			return DEV(i, strtol(mn + strlen(mn), NULL, 10));

	return 0;
}

/* TODO Verbose */
void devices_probe(void)
{
	struct device *devp;

	list_for_each(devp, &devices, l)
		devp->drip->probe(devp);
}

static int dev_probe(struct device *dp)
{
	(void) dp;

	return 0;
}

static void dev_fini(struct device *dp)
{
	(void) dp;

	/* TODO */
}

static struct driver dev_driver = {
	.name	= devname,

	.op	= NULL,
	.probe	= &dev_probe,
	.fini	= &dev_fini
};

int dev_init(void)
{
	return driver_reg(&dev_driver);
}

void dev_exit(void)
{

}

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
		if (devp->dev == (ino_t) ip->inum) {
			ip->mode = I_DEV;
			ip->rdev = devp->dev;
			ip->op = devp->drip->op;

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
		sprintf(nbuf, "%s%d", major_names[MAJOR(devp->dev)],
				MINOR(devp->dev));

		if (strcmp(nbuf, name) != 0)
			continue;

		if (!(cdep = kmalloc(sizeof(struct dirent))))
			return -ENOMEM;

		list_init(&cdep->l);
		list_add(&dp->del, &cdep->l);

		cdep->refs = 1;

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
			if (i++ == fp->off) {
				sprintf(_name, "%s%d",
						major_names[MAJOR(devp->dev)],
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
