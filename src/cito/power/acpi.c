/*
 *
 * Carina
 * ACPI
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

#include <stdlib.h>

#include <kbd.h>
#include <system.h>

#define bit(n) (1 << (n))
#define check_flag(flags, n) ((flags) & bit(n))

void reboot(void)
{
	uint8 trash;

	asm volatile ("cli");

	do {
		trash = io_inb(KBD_CMD);
		if (check_flag(trash, 0) != 0) io_inb(KBD_IO);
	} while (check_flag(trash, 1) != 0);

	io_outb(KBD_CMD, KBD_RESET);

	for (;;) asm volatile ("hlt");
}
