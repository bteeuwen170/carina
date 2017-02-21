/*
 *
 * Elarix
 * src/kernel/dev/video/vga_con.c
 *
 * Copyright (C) 2017 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#include <console.h>
#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

static const char devname[] = "vga_con";

#define VGA_WIDTH	80
#define VGA_HEIGHT	25

static u16 *buf = (u16 *) 0xB8000;

static u8 x = 0, y = 0;
static u8 fg = 0x07, bg = 0x00;

static int state = 0;

static void putc(char c, u8 x, u8 y)
{
	buf[y * VGA_WIDTH + x] = ((u16) c) | ((u16) fg | bg << 4) << 8;
}

void vga_cursor(int mode)
{
	/* TODO */
}

void vga_palette()
{
	/* TODO */
}

void vga_move(int _x, int _y)
{
	u16 l;

	x = _x;
	y = _y;

	l = y * VGA_WIDTH + x;

	io_outb(0x3D4, 0x0E);
	io_outb(0x3D5, l >> 8);
	io_outb(0x3D4, 0x0F);
	io_outb(0x3D5, l);
}

void vga_scroll(int n)
{
	int i;

	/* TODO Implement scrollback buffer */
	if (n < 0)
		return;

	y--;

	for (i = 0; i < (VGA_HEIGHT - n) * VGA_WIDTH; i++)
		buf[i] = buf[i + VGA_WIDTH];

	while (i < VGA_HEIGHT * VGA_WIDTH)
		buf[i++] = ((u16) ' ') | ((u16) fg | bg << 4) << 8;
}

void vga_clear(void)
{
	u8 xi, yi;

	for (xi = 0; xi < VGA_WIDTH; xi++)
		for (yi = 0; yi < VGA_HEIGHT; yi++)
			putc(' ', xi, yi);

	vga_move(0, 0);
}

void vga_putc(const char c)
{
	switch (state) {
	case 0:
		switch (c) {
		case '\b':
			if (!x)
				break;

			x--;
			putc(' ', x, y);
			break;
		case '\n':
			x = 0;
			y++;
			break;
		case '\t':
			x += 8;
			break;
		case '\r':
			break;
		default:
			putc(c, x, y);
			x++;
		}

		if (x >= VGA_WIDTH) {
			x -= VGA_WIDTH;
			y++;
		}

		if (y >= VGA_HEIGHT)
			vga_scroll(1);

		vga_move(x, y);
	}
}

static struct con_ops vga_con_ops = {
	.clear		= &vga_clear,
	.cursor		= &vga_cursor,
	.move		= &vga_move,
	.palette	= NULL,
	.putc		= &vga_putc,
	.scroll		= &vga_scroll
};

static struct con_driver vga_con_driver = {
	.name = devname,

	.op = &vga_con_ops
};

int vga_con_init(void)
{
	int res;

	res = con_reg(&vga_con_driver);
	if (res < 0)
		panic("%s: unable to register console driver (%d)",
				devname, res);

	return 0;
}

void vga_con_exit(void)
{
	/* TODO */
}

MODULE(vga_con, &vga_con_init, &vga_con_exit);
