/*
 *
 * Carina
 * src/kernel/lib/string.h
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

#ifndef _STRING_H
#define _STRING_H

void *memcpy(void *dest, void *src, size_t len);
//void *memmv(void *dest, void *src, size_t len);
char *strcpy(char *dest, const char *src);

char *strcat(char *dest, char *src);

i32 strcmp(const char *str1, const char *str2);

//void *memchr(const void *s, i32 c, size_t len);

void *memset(void *dest, i32 src, size_t len);
size_t strlen(char *str);
char *strrev(char *str);

#endif
