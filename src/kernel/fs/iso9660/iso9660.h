/*
 *
 * Elarix
 * src/kernel/fs/iso9660/iso9660.h
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

#ifndef _ISO9660_H
#define _ISO9660_H

struct iso9660_sb {
	u8	type;
	char	signature[5];
	u8	version;
#if 1
	u8	data[2041];
#else
	u8	reserved0;
	u8	unused0[64];
	u8	reserved1[8];
	/* TODO */
#endif
} __attribute__ ((packed));

struct iso9660_date {
	char	year[4];
	char	month[2];
	char	day[2];
	char	hour[2];
	char	minute[2];
	char	second[2];
	char	centisecond[2];
	i8	timezone;
};

#endif
