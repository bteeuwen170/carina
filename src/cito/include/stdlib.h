/*
 *
 * Carina
 * Header for stdlib Library
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

#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <stdint.h>

#define NULL			((void *) 0)

/* Boolean */
typedef enum {
	false = 0,
	true = 1
} bool;

//TODO Define these myself depending on the architecture
/* Signed Data Types */
typedef int8_t			i8;
typedef int16_t			i16;
typedef int32_t			i32;
typedef int64_t			i64;

typedef int_least8_t	il8;
typedef int_least16_t	il16;
typedef int_least32_t	il32;
typedef int_least64_t	il64;

typedef int_fast8_t		if8;
typedef int_fast16_t	if16;
typedef int_fast32_t	if32;
typedef int_fast64_t	if64;

/* Unsigned Data Types */
typedef uint8_t			u8;
typedef uint16_t		u16;
typedef uint32_t		u32;
typedef uint64_t		u64;

typedef uint_least8_t	ul8;
typedef uint_least16_t	ul16;
typedef uint_least32_t	ul32;
typedef uint_least64_t	ul64;

typedef uint_fast8_t	uf8;
typedef uint_fast16_t	uf16;
typedef uint_fast32_t	uf32;
typedef uint_fast64_t	uf64;

/* Floating-Point Types */
typedef float			f32;
typedef double			f64;

/* String */
typedef char*			string;

#define EXIT_SUCCESS	0x00
#define EXIT_FAILURE	0x01

//i64 hexdec(const string hex);

string itoa(u64 num, const u32 base);

//u64 strtol(const string *str, string *ptr, u32 base);

#endif
