/*
 *
 * Elarix
 * src/kernel/lib/stdlib.h
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

#ifndef _STDLIB_H
#define _STDLIB_H

//void *malloc(size_t size);
//void *calloc(size_t src, size_t size);
//void *realloc(void *ptr, size_t size);
//void *free(void *ptr);
//void *aligned_alloc(size_t align, size_t size);

/* XXX TEMP */
#define HEAP_SIZE (1 * 1024 * 1024)

void *kmalloc(size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

void mm_init(u32 addr, u32 len);
/* XXX / TEMP */

#endif
