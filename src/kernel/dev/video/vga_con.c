/*
 *
 * Elarix
 * src/kernel/dev/video/vga_con.c
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

#include <dev.h>
#include <fs.h>
#include <module.h>

#include <asm/cpu.h>

#include <string.h>

static const char devname[] = "vga_con";

#define VGA_WIDTH	80
#define VGA_HEIGHT	25

static const u8 colors[] = {
	0, 4, 2, 6, 1, 5, 3, 7,
	8, 12, 10, 14, 9, 13, 11, 15
};

static u16 *buf = (u16 *) 0xB8000;

static u8 x, y;
static u8 fg = 0x07, fgm = 0, bg = 0x00;

static int state;
static char esc_buf[42]; /* XXX Safe? */

static void vga_con_putc(char c, u8 x, u8 y)
{
	buf[y * VGA_WIDTH + x] = ((u16) c) | ((u16) fg | bg << 4) << 8;
}

static void vga_con_move(int _x, int _y)
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

static void vga_con_scroll(int n)
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

static void vga_con_clear(void)
{
	u8 xi, yi;

	for (xi = 0; xi < VGA_WIDTH; xi++)
		for (yi = 0; yi < VGA_HEIGHT; yi++)
			vga_con_putc(' ', xi, yi);

	vga_con_move(0, 0);
}

static void vga_con_put(const char c)
{
	char escn_buf[19];
	int i = 0, j = 0;
	long n;

	if (state == 0 && c == '\033') {
		state = 1;
		return;
	} else if (state == 1 && c == '[') {
		switch (c) {
		case '3':
		case '[':
			state = 2;
			return;
		case 'T':
			x += 8;
			break;
		case 'D':
			vga_con_scroll(1);
			return;
		case 'M':
			vga_con_scroll(-1);
			return;
		case '7':
		case '8':
		case '(':
		case ')':
		default:
			state = 0;
			return;
		}
	} else if (state > 1) {
		switch (c) {
		case '2':
			if (state == 2)
				state = 3;
			else
				state = 0;
			return;
		case 'J':
			if (state == 3)
				vga_con_clear();

			state = 0;
			return;
		case 'm':
			while (esc_buf[i]) {
				memset(escn_buf, 0, 19);

				while (esc_buf[i] && esc_buf[i] != ';' &&
						esc_buf[i] != 'm')
					escn_buf[j++] = esc_buf[i++];

				/* TODO Use pointers to avoid extra buffer */
				n = strtol(escn_buf, NULL, 10);

				if (n == 0)
					fgm = 0;
				else if (n == 1)
					fgm = 8;
				else if (n >= 30 && n <= 37)
					fg = colors[n - 30 + fgm];
				else if (n >= 40 && n <= 47)
					bg = colors[n - 40 + fgm];

				if (esc_buf[i] == ';')
					i++;
				j = 0;
			}
		case 'g':
			state = 0;
			return;
		/* TODO Implement other escape seqs. */
		default:
			esc_buf[state++ - 2] = c;
			return;
		}
	} else {
		if (state)
			memset(esc_buf, 0, 42);

		state = 0;
	}

	switch (c) {
	case '\b':
		if (!x)
			break;

		x--;
		vga_con_putc(' ', x, y);
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
		vga_con_putc(c, x, y);
		x++;
	}

	if (x >= VGA_WIDTH) {
		x -= VGA_WIDTH;
		y++;
	}

	if (y >= VGA_HEIGHT)
		vga_con_scroll(1);

	vga_con_move(x, y);
}

static int vga_con_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		vga_con_put(buf[i]);

	return n;
}

static int vga_con_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	/* TODO */
	return 0;
}

static struct file_ops vga_con_file_ops = {
	.write	= &vga_con_write,
	.ioctl	= &vga_con_ioctl
};

static int vga_con_probe(struct device *devp)
{
	u16 off;

	/*
	 * TODO Detect if present (using pci perhaps?)
	 * NOTE Disable vga in qemu using "-vga none"
	 */

	io_outb(0x3D4, 0x0E);
	off = io_inb(0x3D5) << 8;
	io_outb(0x3D4, 0x0F);
	off |= io_inb(0x3D5);

	y = off / 80;
	x = off % 80;

	devp->op = &vga_con_file_ops;

	return 0;
}

static void vga_con_fini(struct device *devp)
{
	/* TODO */
}

static struct driver vga_con_driver = {
	.name	= devname,
	.major	= MAJOR_CON,

	.busid	= BUS_NONE,
	.bus	= NULL,

	.probe	= &vga_con_probe,
	.fini	= &vga_con_fini
};

int vga_con_init(void)
{
	int res;

	if ((res = driver_reg(&vga_con_driver)) < 0)
		return res;

	return device_reg(&vga_con_driver, NULL, 0);
}

void vga_con_exit(void)
{
	driver_unreg(&vga_con_driver);
}

MODULE(vga_con, &vga_con_init, &vga_con_exit);
