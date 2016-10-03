/*
 *
 * Carina
 * src/kernel/include/kernel/kprint.h
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

#ifndef _KERNEL_PRINT_H
#define _KERNEL_PRINT_H

/* Loglevels */
typedef enum {
	KP_DBG	= 0,
	KP_INFO	= 1,
	KP_WARN	= 2,
	KP_ERR	= 3,
	KP_CRIT	= 4
} loglevel_t;

/* Loglevels */
#define LL_DEFAULT	2 //TODO Change to 1
#define LL_QUIET	0 /* KP_ERR + */
#define LL_NORMAL	1 /* KP_WARN + */
#define LL_VERBOSE	2 /* KP_INFO + */
#define LL_DBG		3 /* KP_BDG + */

void kprintf(const loglevel_t kp, char *prefix, char *fmt, ...);

void kprint(char *fmt, ...);

i8 get_kp();

#endif
