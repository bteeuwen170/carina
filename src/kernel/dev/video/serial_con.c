/*
 *
 * Elarix
 * src/kernel/dev/video/serial_con.c
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

#include <errno.h>
#include <dev.h>
#include <fs.h>
#include <module.h>

#include <asm/cpu.h>

#include <string.h>
#include <stdlib.h>

static const char devname[] = "serial_con";

/* TODO Do more testing */
/* Taken from http://www.sci.muni.cz/docs/pc/serport.txt */
static int serial_detect(const u16 addr)
{
	u8 od;

	/* Check for UART presence */
	od = io_inb(addr + 4);

	io_outb(addr + 4, 0x10);
	if (io_inb(addr + 6) & 0xF0)
		return 0;
	io_outb(addr + 4, 0x1F);
	if ((io_inb(addr + 6) & 0xF0) != 0xF0)
		return 0;

	io_outb(addr + 4, od);

	/* Check if 8250 */
	od = io_inb(addr + 7);

	io_outb(addr + 7, 0x55);
	if (io_inb(addr + 7) != 0x55)
		return 1;
	io_outb(addr + 7, 0xAA);
	if (io_inb(addr + 7) != 0xAA)
		return 1;

	io_outb(addr + 7, od);

	/* Check for FIFO */
	io_outb(addr + 2, 0x01);
	od = io_inb(addr + 2);
	io_outb(addr + 2, 0x00);

	/* Check if 8250 or 16450 with scratch regs */
	if (!(od & 0x80))
		return 2;

	/* Check if 16450 */
	if (!(od & 0x40))
		return 3;

	/* Then it's a 16550A */
	return 4;
}

static u16 serial_free(const u16 port)
{
	return io_inb(port + 5) & 0x20;
}

static void serial_out(const u16 port, const char value)
{
	while (!serial_free(port));

	io_outb(port, value);
}

static void serial_con_put(u16 port, const char c)
{
	if (c == '\n')
		serial_out(port, '\r');

	serial_out(port, c);
}

int serial_con_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	struct device *devp;
	size_t i;

	if (!(devp = device_get(fp->dp->rdev)))
		return -ENODEV;

	for (i = 0; i < n; i++)
		serial_con_put(*((u16 *) devp->device), buf[i]);

	return n;
}

static int serial_con_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	/* TODO */
	return 0;
}

static int serial_con_probe(struct device *devp)
{
	return 0;
}

static void serial_con_fini(struct device *devp)
{
	/* TODO */
}

static struct file_ops serial_con_file_ops = {
	.write	= &serial_con_write,
	.ioctl	= &serial_con_ioctl
};

static struct driver serial_con_driver = {
	.name	= devname,

	.op	= &serial_con_file_ops,
	.probe	= &serial_con_probe,
	.fini	= &serial_con_fini
};

int serial_con_init(void)
{
	struct device *devp = NULL;
	u16 port;
	int i, res;

	if ((res = driver_reg(&serial_con_driver)) < 0)
		return res;

	for (i = 0; i < 4; i++) {
		switch (i) {
		case 0:
			port = 0x3F8;
			break;
		case 1:
			port = 0x2F8;
			break;
		case 2:
			port = 0x3E8;
			break;
		case 3:
			port = 0x2E8;
			break;
		}

		if (!serial_detect(port))
			continue;

		if ((res = device_reg(MAJOR_CON, &serial_con_driver, &devp)) <
				0)
			return res;

		if (!(devp->device = kmalloc(sizeof(u16)))) {
			res = -ENOMEM;
			goto err;
		}

		*((u16 *) devp->device) = port;

		io_outb(port + 1, 0x00);
		io_outb(port + 3, 0x80);
		io_outb(port + 0, 0x03);
		io_outb(port + 1, 0x00);
		io_outb(port + 3, 0x03);
		io_outb(port + 2, 0xC7);
		io_outb(port + 4, 0x0B);
	}

	return 0;

err:
	if (devp)
		device_unreg(devp);

	return res;
}

void serial_con_exit(void)
{
	/* TODO */
}

MODULE(serial_con, &serial_con_init, &serial_con_exit);
