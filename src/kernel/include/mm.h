/*
 *
 * Elarix
 * src/kernel/include/mm.h
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

#ifndef _MM_H
#define _MM_H

#define KM_CONT	0x01
/* #define KM_USER	0x02 */

struct mem_info {
	size_t total;
	size_t allocated;
	size_t free;
	size_t used;
};

void *kmalloc(size_t size, int flags);
void kfree(void *addr);

struct mem_info *mm_mem_info(void);
void mm_init(void);

#endif
