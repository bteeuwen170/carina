/*
 *
 * Elarix
 * src/kernel/include/kernel.h
 *
 * Copyright (C) 2017 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#ifndef _KERNEL_H
#define _KERNEL_H

/* Loglevels */
#if 0
#define KP_DBG		"\0010"
#define KP_INFO		"\0011"
#define KP_NOTICE	"\0012"
#define KP_WARN		"\0013"
#define KP_ERR		"\0014"
#define KP_CRIT		"\0015"
#define KP_PANIC	"\0016"

#define KP_CON		"\0017"
#else
#define KP_DBG		""
#define KP_INFO		""
#define KP_NOTICE	""
#define KP_WARN		""
#define KP_ERR		""
#define KP_CRIT		""
#define KP_PANIC	""

#define KP_CON		"  "
#endif

#define KP_DEFAULT	KP_INFO

/* Loglevels */
#define LL_DEFAULT	2 /* TODO Change to 1 */
#define LL_QUIET	0 /* KP_ERR + */
#define LL_NORMAL	1 /* KP_WARN + */
#define LL_VERBOSE	2 /* KP_INFO + */
#define LL_DBG		3 /* KP_BDG + */

void kprint(const char *prefix, char *fmt, ...);

void kprint_init(void); /* XXX TEMP XXX */

#define dprintf(...) 	kprint( __VA_ARGS__)
#define kprintf(...) 	kprint(NULL, __VA_ARGS__)

i8 get_kp();

void panic(char *reason, u32 err_code, uintptr_t ip);

#endif
