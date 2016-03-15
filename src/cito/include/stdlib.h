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

#include <stddef.h>
#include <stdint.h>

#define EXIT_SUCCESS	0x00
#define EXIT_FAILURE	0x01

//int64_t hexdec(const uint8_t *hex);

char *itoa(size_t num, const uint32_t base);

//uint64_t strtol(const char **str, uint8_t **ptr, uint32_t base);

#endif
