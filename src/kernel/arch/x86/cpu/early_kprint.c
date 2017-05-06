/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/early_kprint.c
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

#include <asm/cpu.h>

#define VGA_WIDTH	80
#define VGA_HEIGHT	25

static u16 *vga = (u16 *) 0xB8000;
static u8 x, y;

static void early_kprint_scroll(void)
{
	int i;

	y--;

	for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
		vga[i] = vga[i + VGA_WIDTH];

	while (i < VGA_HEIGHT * VGA_WIDTH)
		vga[i++] = ((u16) ' ') | ((u16) 0x700);
}

/* FIXME Needs a better eco system */
void early_kprint(const char *buf, size_t n)
{
	u16 off;

	while (n--) {
		if (*buf == '\n') {
			x = 0;
			y++;
			buf++;
			continue;
		}

		vga[y * 80 + x++] = ((u16) *buf++) | ((u16) 0x700);
	}

	if (x >= VGA_WIDTH) {
		x -= VGA_WIDTH;
		y++;
	}

	if (y >= VGA_HEIGHT)
		early_kprint_scroll();

	off = y * VGA_WIDTH + x;

	io_outb(0x3D4, 0x0E);
	io_outb(0x3D5, off >> 8);
	io_outb(0x3D4, 0x0F);
	io_outb(0x3D5, off);
}

void early_kprint_init(void)
{
	u16 off;

	io_outb(0x3D4, 0x0E);
	off = io_inb(0x3D5) << 8;
	io_outb(0x3D4, 0x0F);
	off |= io_inb(0x3D5);

	y = off / 80;
	x = off % 80;
}
