/*
 *
 * Elarix
 * src/kernel/dev/timer/timer.c
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
#include <kernel.h>

#include <string.h>

dev_t tmr_dev;

void sleep(const u64 delay)
{
	u64 ticks = 0, target;

	device_read(tmr_dev, &ticks, sizeof(u64));

	target = ticks + delay;

	while (ticks < target) {
		asm volatile ("hlt");

		device_read(tmr_dev, (char *) &ticks, sizeof(u64));
	}
}

void timer_init(void)
{
	char buf[PATH_MAX + 1];

	if (cmdline_str_get("timer", buf) == 0)
		dir_basename(buf);
	else
		strcpy(buf, "tmr0");

	if (!(tmr_dev = device_getbyname(buf)))
		goto err;
	if (!device_get(tmr_dev))
		goto err;

	return;

err:
	panic("unable to initialize a system timer", -ENODEV, 0);
}
