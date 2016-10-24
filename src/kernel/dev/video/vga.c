/*
 *
 * Elara
 * src/kernel/dev/video/vga.c
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

/* TODO Rewrite this crap */

#include <asm/cpu.h>

#include <print.h>

#include "vga.h"

static char *devname = "vga";

static u16 *vga_buffer = (u16 *) 0xB8000;

u16 vga_x, vga_y;

void vga_init(void)
{
	u8 x, y;

	vga_x = vga_y = 0;

	vga_fgcolor = VGA_COLOR_LIGHT_GREY;
	vga_bgcolor = VGA_COLOR_BLACK;

	for (y = 0; y < VGA_HEIGHT; y++)
		for (x = 0; x < VGA_WIDTH; x++)
			 vga_buffer[y * VGA_WIDTH + x] =
					vga_create_entry(' ', vga_fgcolor, vga_bgcolor);

	/* Hide cursor */
	//io_outb(0x3D4, 0x0A);
	//io_outb(0x3D5, 0x1D);

	kprintf(KP_INFO, devname, "early VGA has been initialized at %dx%d\n",
			VGA_WIDTH, VGA_HEIGHT);
}

static void vga_cur_set(void)
{
	u16 l = vga_y * VGA_WIDTH + vga_x;

	io_outb(0x3D4, 0x0E);
	io_outb(0x3D5, l >> 8);
	io_outb(0x3D4, 0x0F);
	io_outb(0x3D5, l);
}

void vga_clear(void)
{
	u8 x, y;

	vga_x = 0;
	vga_y = 0;

	for (y = 0; y < VGA_HEIGHT; y++) {
		for (x = 0; x < VGA_WIDTH; x++) {
			 vga_buffer[y * VGA_WIDTH + x] =
					vga_create_entry(' ', vga_fgcolor, vga_bgcolor);
		}
	}
	
	vga_cur_set();
}

void vga_scroll_down(void)
{
	u16 i;

	vga_y = VGA_HEIGHT - 1;

	for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
		vga_buffer[i] = vga_buffer[i + VGA_WIDTH];

	for (i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
		vga_buffer[i] = vga_create_entry(' ', vga_fgcolor, vga_bgcolor);
}

void vga_putch(char c, u8 color)
{
	if (c != 0) {
		switch (c) {
		case '\b':
			vga_x--;
			vga_buffer[vga_y * VGA_WIDTH + vga_x] =
					vga_create_entry(' ', color, vga_bgcolor); //FIXME New line
			break;
		case '\n':
			vga_x = 0;
			vga_y++;
			break;
		case '\r':
			vga_x = 0; //FIXME Clear line as well!!
		case '\t':
			vga_x += 4; //FIXME New line //FUck
			break;
		default:
			vga_buffer[vga_y * VGA_WIDTH + vga_x] =
					vga_create_entry(c, color, vga_bgcolor);
			vga_x++;
			break;
		}

		if (vga_x >= VGA_WIDTH) {
			vga_x = 0;
			vga_y++;
		}

		if (vga_y >= VGA_HEIGHT)
			vga_scroll_down();

		vga_cur_set();
	}
}
