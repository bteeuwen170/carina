/*
 *
 * Elarix
 * src/kernel/dev/rtc/cmos.c
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
#include <ioctl.h>
#include <kernel.h>
#include <module.h>
#include <time.h>

#include <asm/cpu.h>

static const char devname[] = "cmos";

#define CMOS_CMD	0x70
#define CMOS_IO		0x71

#define CMOS_REG_STATUS	0x0B

#define CMOS_REG_SEC	0x00
#define CMOS_REG_MIN	0x02
#define CMOS_REG_HOUR	0x04

#define CMOS_REG_WDAY	0x06
#define CMOS_REG_MDAY	0x07
#define CMOS_REG_MON	0x08

#define CMOS_REG_YEAR	0x09
#define CMOS_REG_CEN	0x32

static u8 cmos_inb(const u8 reg)
{
	io_outb(CMOS_CMD, reg);

	return io_inb(CMOS_IO);
}

static int cmos_ioctl(struct file *fp, unsigned int cmd, va_list args)
{
	struct tm *tmp;
	int cen;

	(void) fp;

	if (cmd != IO_TIME)
		return -EINVAL;

	tmp = va_arg(args, struct tm *);

	tmp->sec = cmos_inb(CMOS_REG_SEC);
	tmp->min = cmos_inb(CMOS_REG_MIN);
	tmp->hour = cmos_inb(CMOS_REG_HOUR);

	tmp->wday = cmos_inb(CMOS_REG_WDAY);
	tmp->mday = cmos_inb(CMOS_REG_MDAY);
	tmp->mon = cmos_inb(CMOS_REG_MON);

	tmp->yday = 0;
	cen = cmos_inb(CMOS_REG_CEN);
	tmp->year = cmos_inb(CMOS_REG_YEAR);

	if (!(cmos_inb(CMOS_REG_STATUS) & 0x04)) {
		tmp->sec = (tmp->sec & 0x0F) + ((tmp->sec / 16) * 10);
		tmp->min = (tmp->min & 0x0F) + ((tmp->min / 16) * 10);
		tmp->hour = (tmp->hour & 0x0F) + ((tmp->hour / 16) * 10);

		tmp->wday = (tmp->wday & 0x0F) + ((tmp->wday / 16) * 10);
		tmp->mday = (tmp->mday & 0x0F) + ((tmp->mday / 16) * 10);
		tmp->mon = (tmp->mon & 0x0F) + ((tmp->mon / 16) * 10);

		cen = (cen & 0x0F) + ((cen / 16) * 10);
		tmp->year = (tmp->year & 0x0F) + ((tmp->year / 16) * 10);
	}

	if (!cen)
		tmp->year = cen * 100 + tmp->year;
	else
		tmp->year = (tmp->year < 90 ? 2000 : 1900) + tmp->year;

	return 0;
}

static struct file_ops cmos_file_ops = {
	.ioctl = &cmos_ioctl
};

static int cmos_probe(struct device *devp)
{
	devp->op = &cmos_file_ops;

	return 0;
}

static void cmos_fini(struct device *devp)
{
	(void) devp;
}

static struct driver cmos_driver = {
	.name	= devname,
	.major	= MAJOR_RTC,

	.busid	= BUS_NONE,
	.bus	= NULL,

	.probe	= &cmos_probe,
	.fini	= &cmos_fini
};

int cmos_init(void)
{
	struct device *devp;
	int res;

	if ((res = driver_reg(&cmos_driver)) < 0)
		return res;

	if ((res = device_reg(&cmos_driver, &devp, 0)) < 0)
		return res;
	devp->name = "CMOS real time clock";

	return 0;
}

void cmos_exit(void)
{
	driver_unreg(&cmos_driver);
}

MODULE(cmos, &cmos_init, &cmos_exit);
