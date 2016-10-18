/*
 *
 * Carina
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

#include <asm/cputypes.h>

/* Boolean */
typedef enum {
	false =	0,
	true =	1
} bool;

/* NULL */
#define NULL	((void *) 0)

/* Signed data types */
typedef signed char		i8;
typedef signed short		i16;
typedef signed int		i32;
typedef signed long long	i64;

/* Unsigned data types */
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

/* Floating-point types */
//typedef float			f32;
//typedef double		f64;
//typedef long double		f128;

/* IDs */
typedef u32	dev_t;		/* Device ID */
typedef u32	pid_t;		/* Process ID */
typedef u32	uid_t;		/* User ID */
typedef u32	gid_t;		/* Group ID */

/* File system */
typedef i64	off_t;		/* Offset */
typedef i64	time_t;		/* Time in seconds since epoch */
typedef u16	mode_t;		/* Permissions */
typedef u16	link_t;		/* Link count */
typedef u64	fsize_t;	/* File size */
typedef u64	ino_t;		/* Inode number */


/* Addresses */
typedef u32	paddr32_t;	/* Physical address (32-bit) */
typedef u32	vaddr32_t;	/* Virtual address (32-bit) */
typedef u64	paddr64_t;	/* Physical address (64-bit) */
typedef u64	vaddr64_t;	/* Virtual address (64-bit) */
typedef size_t	paddr_t;	/* Physical address */
typedef size_t	vaddr_t;	/* Virtual address */

#endif
