/*
 *
 * Elarix
 * src/kernel/dev/char/serial.c
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

#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

static const char devname[] = "serial";

/* TODO Do more testing */
/* Taken from http://www.sci.muni.cz/docs/pc/serport.txt */
static i8 serial_chip_detect(const u16 addr)
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

u16 serial_read(const u16 port)
{
	return io_inb(port + 5) & 0x01;
}

u16 serial_in(const u16 port)
{
	while (!serial_read(port));

	return io_inb(port);
}

u16 serial_free(const u16 port)
{
	return io_inb(port + 5) & 0x20;
}

void serial_out(const u16 port, const u8 value)
{
	while (!serial_free(port));

	io_outb(port, value);
}

void serial_init(const u16 port)
{
	switch (serial_chip_detect(port)) {
	case 0:
		return;
	case 1:
		dprintf(devname, "%s\n", "detected 8250 serial interface");
		break;
	case 2:
		dprintf(devname, "%s\n", "detected 8250 with scratch regs");
		break;
	case 3:
		dprintf(devname, "%s\n", "detected 16450 serial interface");
		break;
	case 4:
		dprintf(devname, "%s\n", "detected 16550A serial interface");
		break;
	}

	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x80);
	io_outb(port + 0, 0x03);
	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x03);
	io_outb(port + 2, 0xC7);
	io_outb(port + 4, 0x0B);
	//TODO Register the serial input handler
}

/* void serial_exit(void)
{
	[>TODO<]
}

MODULE(serial, &serial_init, &serial_exit); */
