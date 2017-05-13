/*
 *
 * Elarix
 * src/kernel/include/dev.h
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

#ifndef _DEV_H
#define _DEV_H

#include <fs.h>
#include <list.h>

#define DEV(ma, mi)	(((ma) << 8) | (mi))
#define MAJOR(d)	((u32) ((d) >> 8))
#define MINOR(d)	((u32) ((d) & ((1U << 8) - 1)))

#define D_CONTROLLER	1	/* Device is a controller */
#define D_READY		2	/* Device has already been intialized */

#define BUS_NONE	0
#define BUS_PCI		1

#define MAJOR_ZERO	1	/* Zero devices */
#define MAJOR_MEM	2	/* RAM devices */
#define MAJOR_CON	3	/* Consoles */
#define MAJOR_KBD	4	/* Keyboards */
#define MAJOR_MCE	5	/* Mice */
#define MAJOR_DSK	6	/* Disk devices */
#define MAJOR_SND	7	/* Sound hardware */
#define MAJOR_RTC	8	/* Real time clocks */
#define MAJOR_TMR	9	/* System timers */
#define MAJOR_ETC	63	/* Misc. */
#define MAJOR_MAX	64

#define MINOR_MEM_ROOT	0
#define MINOR_MEM_DEV	1

struct device {
	struct list_head l;

	const char	*name;	/* Device name */
	dev_t		dev;	/* Device ID */
	u8		flags;	/* Device flags */

	char	busid[6];	/* Bus ID */
	void	*bus;		/* Bus specific */
	void	*device;	/* Device specific */

	struct driver *drip;	/* Associated driver pointer */

	struct file_ops	*op;	/* Device operations */
};

struct driver {
	struct list_head l;

	const char	*name;	/* Driver name */
	u32		major;	/* Major */

	int		busid;	/* Bus type */
	const void	*bus;	/* Bus specific */

	/* Probe device: dp */
	int (*probe) (struct device *);
	/* Finalize device: dp */
	void (*fini) (struct device *);
};

extern const char *dev_names[];

extern struct list_head drivers;
extern struct list_head devices;

int driver_reg(struct driver *drip);
void driver_unreg(struct driver *drip);

int device_reg(struct driver *drip, struct device **devp, u8 flags);
void device_unreg(struct device *devp);
struct device *device_get(dev_t dev);
dev_t device_getbyname(const char *name);
void devices_probe(void);

int device_read(dev_t dev, char *buf, size_t n);
int device_write(dev_t dev, const char *buf, size_t n);
int device_ioctl(dev_t dev, unsigned int cmd, ...);

#endif
