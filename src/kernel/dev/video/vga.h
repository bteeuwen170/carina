/*
 *
 * Carina
 * Header for VGA Driver
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

#ifndef __VGA_H__
#define __VGA_H__


#define VGA_WIDTH		80
#define VGA_HEIGHT		25

enum vga_colors {
	COLOR_BLACK			= 0x00,
	COLOR_BLUE			= 0x01,
	COLOR_GREEN			= 0x02,
	COLOR_CYAN			= 0x03,
	COLOR_RED			= 0x04,
	COLOR_MAGENTA		= 0x05,
	COLOR_BROWN			= 0x06,
	COLOR_LIGHT_GREY	= 0x07,
	COLOR_DARK_GREY		= 0x08,
	COLOR_LIGHT_BLUE	= 0x09,
	COLOR_LIGHT_GREEN	= 0x0A,
	COLOR_LIGHT_CYAN	= 0x0B,
	COLOR_LIGHT_RED		= 0x0C,
	COLOR_LIGHT_MAGENTA	= 0x0D,
	COLOR_LIGHT_BROWN	= 0x0E,
	COLOR_WHITE			= 0x0F
};

enum vga_cursors {
	CURSOR_GONE			= 0x00,
	CURSOR_FLAT			= 0x01,
	CURSOR_BLOCK		= 0x02
};

u8 vga_fgcolor, vga_bgcolor;

void vga_init(void);

void vga_clear(void);

void vga_scroll_down(void);

void vga_putch(char c, u8 color);

static inline u16 vga_create_entry(const u8 character,
								   const u8 fgcolor,
								   const u8 bgcolor)
{
	u16 _character = character;
	u16 _color = fgcolor | bgcolor << 4;
	return _character | _color << 8;
}

#endif