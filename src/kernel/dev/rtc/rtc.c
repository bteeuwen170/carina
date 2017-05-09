/*
 *
 * Elarix
 * src/kernel/dev/rtc/rtc.c
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

#include <cmdline.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <ioctl.h>
#include <kernel.h>
#include <time.h>

#include <string.h>

dev_t rtc_dev;

int rtc_gettime(time_t *tp)
{
	struct tm tm;
	int res;

	if ((res = device_ioctl(rtc_dev, IO_TIME, &tm)) < 0)
		return res;

	/* TODO */

	*tp = 0;

	return 0;
}

int rtc_gettm(struct tm *tmp)
{
	return device_ioctl(rtc_dev, IO_TIME, tmp);
}

void rtc_init(void)
{
	char buf[PATH_MAX + 1];

	if (cmdline_str_get("rtc", buf) == 0)
		dir_basename(buf);
	else
		strcpy(buf, "rtc0");

	if (!(rtc_dev = device_getbyname(buf)))
		goto err;
	if (!device_get(rtc_dev))
		goto err;

	return;

err:
	panic("unable to initialize a real time clock", -ENODEV, 0);
}
