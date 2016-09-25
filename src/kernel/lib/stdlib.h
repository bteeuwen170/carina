/*
 *
 * Carina
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

#ifndef __STDLIB_H_
#define __STDLIB_H_

/* Type conversion */
char *itoa(u64 num, const u32 base);
//u64 strtol(const string *str, string *ptr, u32 base);

/* Pseudo-random sequence generation */

/* Memory allocation and deallocation */
#define HEAP_SIZE			(1 * 1024 * 1024)

//void *heap = NULL;
//void 

void *malloc(u64 size);
void *kmalloc(u64 size);
//void *calloc(u64 src, u64 size);
//void *realloc(void *ptr, u64 size);
void kfree(void *ptr);

void mm_init(void);

/* Process control */

/* Sorting, searching and comparison */

/* Mathematics */ //XXX move to math.h

#endif
