/*
 *
 * Elarix
 * src/kernel/include/console.h
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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <list.h>

#define CUR_BLOCK	0
#define CUR_VBAR	1
#define CUR_HBAR	2

struct con_driver {
	struct list_head l;

	const char *name;

	int (*probe) (void);
	void (*fini) (void);
	void (*write) (const char);
};

int con_reg(struct con_driver *driver);
void con_unreg(struct con_driver *driver);

#endif
