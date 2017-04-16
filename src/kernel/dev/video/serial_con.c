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

#include <console.h>
#include <fs.h>
#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

#include <string.h>

static const char devname[] = "serial_con";

static int serial_minor[] = { 0, 0, 0, 0 };

/* TODO Do more testing */
/* Taken from http://www.sci.muni.cz/docs/pc/serport.txt */
static i8 serial_detect(const u16 addr)
{
	i32 od;

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

static void serial_con_write(u16 minor, const char c)
{
	u16 port;

	if (minor == serial_minor[0])
		port = 0x3F8;
	else if (minor == serial_minor[1])
		port = 0x2F8;
	else if (minor == serial_minor[2])
		port = 0x3E8;
	else if (minor == serial_minor[3])
		port = 0x2E8;
	else
		return;

	if (c == '\n')
		serial_out(port, '\r');

	serial_out(port, c);
}

static int serial_con_probe(void)
{
	int i;
	u16 port;

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

		switch (serial_detect(port)) {
		case 1:
			dprintf(devname, "%s\n",
					"detected 8250 serial interface");
			break;
		case 2:
			dprintf(devname, "%s\n",
					"detected 8250 with scratch regs");
			break;
		case 3:
			dprintf(devname, "%s\n",
					"detected 16450 serial interface");
			break;
		case 4:
			dprintf(devname, "%s\n",
					"detected 16550A serial interface");
			break;
		default:
			continue;
		}

		io_outb(port + 1, 0x00);
		io_outb(port + 3, 0x80);
		io_outb(port + 0, 0x03);
		io_outb(port + 1, 0x00);
		io_outb(port + 3, 0x03);
		io_outb(port + 2, 0xC7);
		io_outb(port + 4, 0x0B);

		serial_minor[i] = ++console_minor_last;
		dev_init((dev_t) { MAJOR_CON, serial_minor[i] });
	}

	return 0;
}

static void serial_con_fini(u16 minor)
{
	/* TODO */
}

static struct con_driver serial_con_driver = {
	.name	= devname,

	.probe	= &serial_con_probe,
	.fini	= &serial_con_fini,
	.write	= &serial_con_write
};

int serial_con_init(void)
{
	int res;

	res = con_reg(&serial_con_driver);
	if (res < 0)
		kprintf("%s: unable to register console driver (%d)",
				devname, res);

	return 0;
}

void serial_con_exit(void)
{
	/* TODO */
}

MODULE(serial_con, &serial_con_init, &serial_con_exit);
MODULE_BEFORE(con);
