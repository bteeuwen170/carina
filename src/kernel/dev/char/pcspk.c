/*
 *
 * Elarix
 * src/kernel/dev/char/pcspk.c
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

#include <module.h>

#include <asm/cpu.h>

#include <timer/pit.h>

void pcspk_play(const u16 freq)
{
	u16 val;
	u8 spkr;

	val = PIT_FREQ / freq;

	io_outb(PIT_CMD, PIT_SPKR);
	io_outb(PIT_CH2_IO, val & 0xFF);
	io_outb(PIT_CH2_IO, (val >> 8) & 0xFF);

	spkr = io_inb(0x61);
	if (spkr != (spkr | 3))
		io_outb(0x61, spkr | 3);
}

void pcspk_stop(void)
{
	u8 spkr;

	spkr = io_inb(0x61) & 0xFC;
	io_outb(0x61, spkr);
}

MODULE(pcspk, NULL, NULL);
