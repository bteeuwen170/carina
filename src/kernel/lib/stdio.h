/*
 *
 * Carina
 * Header for stdio Library
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

#ifndef _STDIO_H
#define _STDIO_H


//TODO This is crap
#define FAIL	-1
#define NONE	0
#define OK	1

#define EOF	-1

/*struct file {
	i32	flags;
	char	*read_base;
	char	*read_ptr;
	char	*read_end;
	char	*write_base;
	char	*write_ptr;
	char	*write_end;
	char	*buf_base;
	char	*buf_end;
	i64	size;
	i64	len;
};*/

void panic(char *reason, u32 err_code);

void printc(char c);
void printcc(char c, u8 color);

void prints(char *str);

char getch(void);

#endif
