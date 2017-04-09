/*
 *
 * Elarix
 * src/kernel/lib/ctype.c
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

#include "ctype.h"

int isalnum(char c)
{
	return isalpha(c) || isdigit(c);
}

int isalpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isblank(char c)
{
	return c == ' ' || c == '\t';
}

int isdigit(char c)
{
	return c >= '0' && c <= '9';
}

int islower(char c)
{
	return c >= 'a' && c <= 'z';
}

int isprint(char c)
{

}

int ispunct(char c)
{

}

int isspace(char c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' ||
		c == '\t' || c == '\v';
}

int isupper(char c)
{
	return c >= 'A' && c <= 'Z';
}

int isxdigit(char c)
{
	return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

char tolower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);

	return c;
	while (*s) {
		if (('A' <= *s) && *s <= 'Z')
			*s = 'a' + (*s - 'A');

		s++;
	}

	return 0;
}

char *stolower(char *str)
{
	for (; *str; str++)
		*str = tolower(*str);

	return str;
}
