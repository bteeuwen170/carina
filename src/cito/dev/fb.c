/*
 *
 * Carina
 * Framebuffer Driver
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

#include <stdint.h>

#include <fb.h>
#include <system.h>
#include <vga.h>

static uint16_t *VGA_MEMORY = (uint16_t *) 0xB8000;

void fb_init(const uint8_t fb)
{
	fb_x[fb_focus] = 0;
	fb_y[fb_focus] = 0;

	//if (!fb_buffers[fb_focus]) memcpy(fb_buffers[fb_focus], VGA_MEMORY, sizeof(VGA_MEMORY));
	//memcpy(VGA_MEMORY, fb_buffers[fb_focus], sizeof(fb_buffers[fb_focus])); //TODO Do on fb switch not init
	
	fb_focus = fb;
	fb_buffer = VGA_MEMORY;

	fb_fgcolor = COLOR_LIGHT_GREY; //TODO Individual fb colors
	fb_bgcolor = COLOR_BLACK;

	for (uint8_t y = 0; y < VGA_HEIGHT; y++)
		for (uint8_t x = 0; x < VGA_WIDTH; x++)
			 fb_buffer[y * VGA_WIDTH + x] =
					vga_create_entry(' ', fb_fgcolor, fb_bgcolor);
}

/*void fb_systembar_set(void)
{
	//TODO WIP
	
	fb_y[fb_focus] = 1;
	for (uint16_t i = 0; i < VGA_WIDTH; i++)
		fb_buffer[i] = vga_create_entry(' ', 0, COLOR_LIGHT_GREY);
}*/

void fb_cur_set(void)
{
	uint16_t location = fb_y[fb_focus] * VGA_WIDTH + fb_x[fb_focus];

	io_outb(0x3D4, 0x0E);
	io_outb(0x3D5, location >> 8);
	io_outb(0x3D4, 0x0F);
	io_outb(0x3D5, location);
}

void fb_cur_style(const uint8_t style)
{
	switch (style) {
		case CURSOR_GONE:
			io_outb(0x3D4, 0x0A);
			io_outb(0x3D5, 0x20);
			break;
		case CURSOR_FLAT:
			io_outb(0x3D4, 0x0A);
			io_outb(0x3D5, 0x0E);
			break;
		case CURSOR_BLOCK:
			io_outb(0x3D4, 0x0A);
			io_outb(0x3D5, 0x00);
			break;
	}
}

void fb_clr(void)
{
	fb_x[fb_focus] = 0;
	fb_y[fb_focus] = 0;

	for (uint8_t y = 0; y < VGA_HEIGHT; y++) {
		for (uint8_t x = 0; x < VGA_WIDTH; x++) {
			 fb_buffer[y * VGA_WIDTH + x] =
					vga_create_entry(' ', fb_fgcolor, fb_bgcolor);
		}
	}
	
	fb_cur_set();
}

void fb_scrl_dwn(void)
{
	fb_y[fb_focus] = VGA_HEIGHT - 1;

	for (uint16_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) fb_buffer[i] =
		 fb_buffer[i + VGA_WIDTH];

	for (uint16_t i = (VGA_HEIGHT - 1) * VGA_WIDTH;
		 i < VGA_HEIGHT * VGA_WIDTH; i++)
		fb_buffer[i] = vga_create_entry(' ', fb_fgcolor, fb_bgcolor);
}
