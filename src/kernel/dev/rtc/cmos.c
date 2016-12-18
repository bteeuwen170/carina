/*
 *
 * Elara
 * src/kernel/dev/rtc/cmos.c
 *
 * Copyright (C) 2016 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#include <asm/cpu.h>

#include <kernel.h>

#include "cmos.h"

static const char devname[] = "rtc";

/* TODO Make get_time function + check if not updating using CMOS_A */
static u8 cmos_in(const u8 reg)
{
	io_outb(CMOS_CMD, reg);

	return io_inb(CMOS_IO);
}

void rtc_init(void)
{
	dprintf(devname,
			"current time is %02x%02x/%02x/%02x %02x:%02x:%02x UTC\n",
			cmos_in(CMOS_CENTURY), cmos_in(CMOS_YEAR),
			cmos_in(CMOS_MONTH), cmos_in(CMOS_DAY),
			cmos_in(CMOS_HOURS), cmos_in(CMOS_MINUTES),
			cmos_in(CMOS_SECONDS));
}
