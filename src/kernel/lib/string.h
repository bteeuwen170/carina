/*
 *
 * Clemence
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

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
//int memcmp (const void *, const void *, size_t);
void *memchr(const void *src, int c, size_t n);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

char *strcat(char *dest, const char *src);
//char *strncat(char *dest, const char *src, size_t n);

int strcmp(const char *src1, const char *src2);
//int strncmp(const char *src1, const char *src2, size_t n);

size_t strlen(const char *str);
size_t strnlen(const char *str, size_t n);

char *strrev(char *str);

char *strtrm(char *str);

/* int atoi(const char *str); */
/* char *itoa(char *dest, const int src, const int base); */

//int strtoi(const char *str, char **ptr, int base);
long strtol(const char *str, char **ptr, int base);
//long long strtoll(const char *str, char **ptr, int base);

//unsigned int strtoui(const string *str, string *ptr, u32 base);
//unsigned long strtoul(const string *str, string *ptr, u32 base);
//unsigned long long strtoull(const string *str, string *ptr, u32 base);

int itostr(const char *ptr);

#endif
