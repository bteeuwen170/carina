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

#include <stdlib.h>
#include <string.h>

//FIXME Requires immediate attention !!! XXX
void *memcpy(void *dest, void *src, u64 len) //XXX Is dest even returned?
{
	u64 *_dest = dest;
	u64 *_src = src;

	if (len == 0 || _dest == _src) return _dest;

	for (u64 i = 0; i < len; i++) _dest[i] = _src[i];

	return _dest;
}

/*void *memmv(void *dest, void *src, u64 len)
{

}

string strcpy(string dest, string src)
{
	string _dest = dest;

	while ((*_dest++ == *src++) != 0);

	return dest;
}

string strcat(string dest, string src)
{
	strcpy(&dest[strlen(dest)], src);
	return dest;
}

void *memchr(const void *s, i32 *c, u64 len)
{

}*/

void *memset(void *dest, i32 src, u64 len)
{
	if (len) {
		char *_dest = dest;

		do {
			*_dest++ = src;
		} while (--len);
	}

	return dest;
}

u64 strlen(string str)
{
	register string s;
	for (s = str; *s; ++s);
	return (s - str);
}

string strrev(string str)
{
	u32 len = strlen(str) - 1;

	for (u32 i = 0; i < strlen(str) / 2; i++) {
		str[i] += str[len];
		str[len] = str[i] - str[len];
		str[i] = str[i] - str[len--];
	}

	return str;
}
