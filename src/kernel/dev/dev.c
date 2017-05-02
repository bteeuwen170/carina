/*
 *
 * Elarix
 * src/kernel/dev/dev.c
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
#include <kernel.h>
#include <limits.h>
#include <list.h>
#include <pci.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "dev";

static u32 minor_last[MAJOR_MAX] = { 0 };

const char *dev_names[] = {
	NULL,
	"zero", "mem", "con", "kbd", "mce", "dsk", "snd", "rtc",
	[63] = "etc"
};

LIST_HEAD(drivers);
LIST_HEAD(devices);

int driver_reg(struct driver *drip)
{
	struct driver *cdrip;

	if (!drip)
		return -EINVAL;

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

int device_reg(struct driver *drip, struct device **devp, u8 flags)
{
	struct device *cdevp;

	if (!drip)
		return -EINVAL;

	if (!(cdevp = kcalloc(1, sizeof(struct device))))
		return -ENOMEM;

	list_init(&cdevp->l);
	list_add(&devices, &cdevp->l);

	cdevp->name = "Generic device";
	cdevp->dev = DEV(drip->major,
			flags & D_CONTROLLER ? 0 : minor_last[drip->major]++);
	cdevp->flags = flags;

	cdevp->drip = drip;

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

	list_for_each(devp, &devices, l) {
		if (devp->dev == dev) {
			if (devp->flags & D_CONTROLLER)
				return NULL;
			else
				return devp;
		}
	}

	return NULL;
}

dev_t device_getbyname(const char *name)
{
	char *mn;
	unsigned int i;

	for (i = 0; i < sizeof(dev_names) / sizeof(dev_names[0]); i++)
		if (dev_names[i] && (mn = strstr(name, dev_names[i])))
			return DEV(i, strtol(mn + strlen(mn), NULL, 10));

	return 0;
}

void devices_probe(void)
{
	struct device *devp;
	int res;

	list_for_each(devp, &devices, l) {
		if (devp->flags & D_CONTROLLER)
			dprintf("%s (dev %d) (%s)\n",
					devp->name, MAJOR(devp->dev),
					devp->drip->name);
		else
			dprintf("%s (dev %d, %d) (%s)\n",
					devp->name,
					MAJOR(devp->dev), MINOR(devp->dev),
					devp->drip->name);

		if ((res = devp->drip->probe(devp)) < 0) {
			dprintf(KP_ERR KP_CON
					" failed to initialize (%d)\n", res);
			continue;
		}
	}
}

#if 0
static int dev_probe(struct device *devp)
{
	(void) devp;

	return 0;
}

static void dev_fini(struct device *devp)
{
	(void) devp;

	/* TODO */
}

static struct driver dev_driver = {
	.name	= devname,

	.op	= NULL,
	.probe	= &dev_probe,
	.fini	= &dev_fini
};
#endif

int dev_init(void)
{
	/* return driver_reg(&dev_driver); */
	return 0;
}

void dev_exit(void)
{

}
