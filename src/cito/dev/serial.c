/*
 *
 * Carina
 * Serial Interface Driver
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

#include <system.h>

void serial_init(const uint16_t port) //TODO Constants
{
	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x80);
	io_outb(port + 0, 0x03);
	io_outb(port + 1, 0x00);
	io_outb(port + 3, 0x03);
	io_outb(port + 2, 0xC7);
	io_outb(port + 4, 0x0B);

	//TODO Register the serial input handler
}

uint16_t serial_read(const uint16_t port)
{
	return io_inb(port + 5) & 0x01;
}

uint16_t serial_in(const uint16_t port)
{
	while (serial_read(port) == 0);
	return io_inb(port);
}

uint16_t serial_free(const uint16_t port)
{
	return io_inb(port + 5) & 0x20;
}

void serial_out(const uint16_t port, const uint8_t value)
{
	while (serial_free(port) == 0);
	io_outb(port, value);
}
