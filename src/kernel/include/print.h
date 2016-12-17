/*
 *
 * Elara
 * src/kernel/include/print.h
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

#ifndef _PRINT_H
#define _PRINT_H

/* Loglevels */
#define KP_DBG		1
#define KP_INFO		2
#define KP_WARN		4
#define KP_ERR		8
#define KP_CRIT		16
#define KP_CON		32

/* Loglevels */
#define LL_DEFAULT	2 //TODO Change to 1
#define LL_QUIET	0 /* KP_ERR + */
#define LL_NORMAL	1 /* KP_WARN + */
#define LL_VERBOSE	2 /* KP_INFO + */
#define LL_DBG		3 /* KP_BDG + */

void kprintf(const u8 kp, const char *prefix, char *fmt, ...);

i8 get_kp();

#endif
