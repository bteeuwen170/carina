/*
 *
 * Elarix
 * src/kernel/init/cmdline.c
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

#include <kernel.h>

#include <string.h>

static const char devname[] = "cmdline";

/* XXX How big should this buffer be? */
char cmdline[4096];

/* int cmdline_bool_get(char *param, int *val)
{
	return 1;
} */

/* int cmdline_long_get(char *param, long *val)
{
	[>TODO<]
	return 1;
} */

int cmdline_str_get(char *param, char *val)
{
	char *p;

	if (!(p = strstr(cmdline, param)))
		return 1;

	while (*p && *p != '=')
		if (*p++ == ' ')
			return 1;

	for (p++; *p && *p != ' '; p++)
		if (*p == '=')
			return 1;
		else
			*val++ = *p;

	return 0;
}

void cmdline_init(const char *str)
{
	strncpy(cmdline, str, 4096);
	dprintf("%s\n", cmdline);
}
