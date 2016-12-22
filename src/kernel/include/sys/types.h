/*
 *
 * Elarix
 * src/kernel/include/sys/types.h
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

#ifndef _TYPES_H
#define _TYPES_H

#include <asm/sys/types.h>

#define NULL	((void *) 0)

typedef __asm_i8	i8;
typedef __asm_i16	i16;
typedef __asm_i32	i32;
typedef __asm_i64	i64;
typedef __asm_ssize_t	ssize_t;

typedef __asm_u8	u8;
typedef __asm_u16	u16;
typedef __asm_u32	u32;
typedef __asm_u64	u64;
typedef __asm_size_t	size_t;

//typedef float		f32;
//typedef double	f64;
//typedef long double	f128;

typedef u32		pid_t;		/* Process ID */
typedef u32		uid_t;		/* User ID */
typedef u32		gid_t;		/* Group ID */

typedef i64		off_t;		/* Offset */
typedef i64		time_t;		/* Time in seconds since epoch */
typedef u16		mode_t;		/* Permissions */
typedef u16		link_t;		/* Link count */
typedef u64		ino_t;		/* Inode number */

typedef u32		intptr32_t;	/* Address (32-bit) */
typedef u64		intptr64_t;	/* Address (64-bit) */
typedef __asm_intptr_t	intptr_t;	/* Address */
typedef __asm_uintptr_t	uintptr_t;	/* Unsigned address */

typedef struct {
	u8	major;
	u16	minor;
} dev_t;				/* Device ID */

#endif
