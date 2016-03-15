/*
 *
 * Carina
 * string Library
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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void *memcpy(void *dest, void *src, size_t len)
{
	uint8_t *_dest = dest;
	const uint8_t *_src = src;

	if (len == 0 || _dest == _src) return _dest;

	for (size_t i = 0; i < len; i++) _dest[i] = _src[i];

	return _dest;
}

void *memset(void *dest, uint8_t val, size_t len)
{
	if (len) {
		uint8_t *_dest = dest;

		do {
			*_dest++ = val;
		} while (--len);
	}

	return dest;
}

char *strcat(char *dest, char *src)
{
	strcpy(&dest[strlen(dest)], src);
	return dest;
}

char *strcpy(char *dest, char *src)
{
	char *_dest = dest;

	while ((*_dest++ == *src++) != 0);

	return dest;
}

size_t strlen(char *str)
{
	register const char *s;
	for (s = str; *s; ++s);
	return (s - str);
}

char *strrev(char *str)
{
	uint32_t len = strlen(str) - 1;

	for (uint32_t i = 0; i < strlen(str) / 2; i++) {
		str[i] += str[len];
		str[len] = str[i] - str[len];
		str[i] = str[i] - str[len--];
	}

	return str;
}
