/*
 *
 * Carina
 * Time
 *
 * Copyright (C) 2015 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#include <cmos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

i8 time_init(void)
{
	return OK;
}

void time_nice(void *str)
{
	static char weekdays[7][3] = {
		"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
	};

	static char months[12][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		"Aug", "Sep", "Oct", "Nov", "Dec"
	};

	memcpy(&str, "test", 4);
	//char str[26];
	/*strcpy(str, weekdays[7]);
	strcat(str, " ");
	strcat(str, months[cmos_in(CMOS_MONTH), 10]);
	strcat(str, " ");
	strcat(str, itoa(cmos_in(CMOS_DAY), 10));
	strcat(str, " ");
	strcat(str, itoa(cmos_in(CMOS_HOURS), 10));
	strcat(str, ":");
	strcat(str, itoa(cmos_in(CMOS_MINUTES), 10));
	strcat(str, ":");
	strcat(str, itoa(cmos_in(CMOS_SECONDS), 10));
	strcat(str, " ");
	strcat(str, "UTC");
	strcat(str, " ");
	strcat(str, itoa(cmos_in(CMOS_YEAR), 10));*/
}
