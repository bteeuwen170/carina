/*
 *
 * Elara
 * src/kernel/lib/string.c
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

#include "string.h"

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	for (; n; n--)
		*d++ = *s++;

	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;

	if (d == s)
		return dest;

	if (s > d) {
		while (n--)
			*d++ = *s++;
	} else {
		d += n - 1;
		s += n - 1;

		while (n--)
			*d-- = *s--;
	}

	return dest;
}

void *memset(void *dest, int c, size_t n)
{
	unsigned char *d = dest;

	n++;

	while (--n)
		*d++ = c;

	return dest;
}

//TODO memcmp

void *memchr(const void *src, int c, size_t n)
{
	const unsigned char *s = src;

	for (; n--; s++)
		if (*s == c)
			return (void *) s;

	return 0;
}

char *strcpy(char *dest, const char *src)
{
	char *d = dest;
	const char *s = src;

	while ((*d++ = *s++));

	return dest;
}

/* FIXME Does this work like it's supposed to? */
char *strncpy(char *dest, const char *src, size_t n)
{
#if 1
	/* Copied from "man strncpy" */
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];

	while (i < n)
		dest[i++] = '\0';

	return dest;
#else
	char *d = dest;

	while (n-- && (*dest++ = *src++));

	return d;
#endif
}

char *strcat(char *dest, const char *src)
{
	strcpy(dest + strlen(dest), src);

	return dest;
}

//TODO strncat

int strcmp(const char *src1, const char *src2)
{
	while (*src1 == *src2 && *src1) {
		src1++;
		src2++;
	}

	return *(unsigned char *) src1 - *(unsigned char *) src2;
}

//TODO strncmp

size_t strlen(const char *str)
{
	const char *s = str;

	while (*s)
		s++;

	return (s - str);
}

size_t strnlen(const char *str, size_t n)
{
	size_t l;

	for (l = 0; *str && l < n; l++, str++);

	return l;
}

char *strrev(char *str)
{
	unsigned int i;
	unsigned int n = strlen(str) - 1;

	for (i = 0; i < strlen(str) / 2; i++) {
		str[i] += str[n];
		str[n] = str[i] - str[n];
		str[i] = str[i] - str[n--];
	}

	return str;
}

char *strtrm(char *str)
{
	char *d;

	while (*str == ' ')
		str++;

	d = str + strlen(str);

	while (*--d == ' ')
		*d = '\0';

	return str;
}

long strtol(const char *str, char **ptr, int base)
{
	long n;

	if (*str == '+' || *str == '-')
		str++;

	if (base == 0) {
		//TODO
	}

	for (;;) {
		if ((*str >= '0' && *str <= '7') ||
				((*str == '8' || *str == '9') && base > 8))
			n = n * base + *(str++) - '0';
		else if (((*str >= 'a' && *str <= 'f') ||
				(*str >= 'A' && *str <= 'F')) && base == 16)
			n = n * base + *(str++) - 'A' + 10;
		else
			break;
	}

	str++;

	if (!*ptr)
		*ptr = (char *) str;
	//TODO Memcpy

	return (*str == '-' ? -1 : 1) * n;
}

//TODO See header file

int itostr(const char *ptr)
{
	int n = 0, sign = -1;

	while (*ptr == ' ')
		ptr++;

	switch (*ptr) {
	case '+':
		sign = 1;
	case '-':
		ptr++;
	}

	while ((unsigned int) (*ptr - '0') < 10)
		n = 10 * n - (*ptr++ - '0');

	return n * sign;
}
