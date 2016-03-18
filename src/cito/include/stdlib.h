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

/* Unsigned Data Types */
typedef uint8_t			uint8;
typedef uint16_t		uint16;
typedef uint32_t		uint32;
typedef uint64_t		uint64;

/* Signed Data Types */
typedef int8_t			int8;
typedef int16_t			int16;
typedef int32_t			int32;
typedef int64_t			int64;

/* Floating-Point Types */
typedef float			float32;
typedef double			float64;

/* String */
//typedef int8			char;
typedef char*			string;

#define EXIT_SUCCESS	0x00
#define EXIT_FAILURE	0x01

//int64 hexdec(const string hex);

string itoa(uint64 num, const uint32 base);

//uint64 strtol(const string *str, string *ptr, uint32 base);

#endif
