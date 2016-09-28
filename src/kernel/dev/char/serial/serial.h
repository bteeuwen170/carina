/*
 *
 * Carina
 * src/kernel/dev/char/serial/serial.h
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

#ifndef _SERIAL_H
#define _SERIAL_H

#define COM0	0x3F8
#define COM1	0x2F8
#define COM2	0x3E8
#define COM3	0x2E8

void serial_init(const u16 port);

u16 serial_in(const u16 port);

void serial_out(const u16 port, const u8 value);

#endif
