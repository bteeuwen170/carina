/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/reboot.c
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

#define PS2_CMD		0x64
#define PS2_IO		0x60

#define PS2_RESET	0xFE

#define bit(n) (1 << (n))
#define check_flag(flags, n) ((flags) & bit(n))

void reboot(void)
{
	u8 r;

	asm volatile ("cli");

	do {
		r = io_inb(PS2_CMD);
		if (r & 1)
			io_inb(PS2_IO);
	} while (r & 2);

	io_outb(PS2_CMD, PS2_RESET);
}
