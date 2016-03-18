/*
 *
 * Carina
 * stdio Library
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

#include <fb.h>
#include <kbd.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>

void printc(char c)
{
	printcc(c, fb_fgcolor);
}

void printcc(char c, uint8 color)
{
	if (c != 0) {
		switch (c) {
			case '\b':
				fb_x[fb_focus]--;
				fb_buffer[fb_y[fb_focus] * VGA_WIDTH + fb_x[fb_focus]] =
						vga_create_entry(' ', color, fb_bgcolor); //FIXME New line
				break;
			case '\n':
				fb_x[fb_focus] = 0;
				fb_y[fb_focus]++;
				break;
			case '\r':
				fb_x[fb_focus] = 0; //FIXME Clear line as well!!
			case '\t':
				fb_x[fb_focus] += TAB_SIZE; //FIXME New line
				break;
			default:
				fb_buffer[fb_y[fb_focus] * VGA_WIDTH + fb_x[fb_focus]] =
						vga_create_entry(c, color, fb_bgcolor);
				fb_x[fb_focus]++;
				break;
		}

		if (fb_x[fb_focus] >= VGA_WIDTH) {
			fb_x[fb_focus] = 0;
			fb_y[fb_focus]++;
		}

		if (fb_y[fb_focus] >= VGA_HEIGHT) fb_scrl_dwn();

		fb_cur_set();
	}
}

void prints(string str)
{
	uint32 length = 0;
	while (str[length]) printc(str[length++]);
}

void printsc(string str, uint8 color)
{
	uint32 length = 0;
	while (str[length]) printcc(str[length++], color);
}

/*void scans(char *ptr)
{
	for (;;) {
		if (kbuf == '\n') break;
	}
}*/
