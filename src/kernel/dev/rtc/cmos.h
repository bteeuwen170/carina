/*
 *
 * Clemence
 * src/kernel/dev/rtc/cmos.h
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

#ifndef _CMOS_H
#define _CMOS_H

#define CMOS_CMD	0x70
#define CMOS_IO		0x71

enum cmos_registers {
	CMOS_CENTURY	= 0x32,
	CMOS_YEAR	= 0x09,
	CMOS_MONTH	= 0x08,
	CMOS_DAY	= 0x07,
	CMOS_HOURS	= 0x04,
	CMOS_MINUTES	= 0x02,
	CMOS_SECONDS	= 0x00,
	CMOS_A		= 0x0A,
	CMOS_B		= 0x0B,
	CMOS_C		= 0x0C,
	CMOS_D		= 0x0D,
	CMOS_CONFIG	= 0x11,
	CMOS_DIAG	= 0x0E,
	CMOS_PERIF	= 0x14
};

void rtc_init(void);

#endif
