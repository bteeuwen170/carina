/*
 *
 * Carina
 * PIT Driver
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

#include <system.h>

u64 ticks;

volatile bool done;
u64 target;

u64 uptime(void)
{
	return ticks;
}

void pit_handler(registers_t *registers)
{
	(void) registers;
	ticks++;

	if (!done && target) target--;
	if (target == 0) done = true;
}

void pit_init(void)
{
	u16 freq = PIT_FREQ / 1000;

	io_outb(PIT_IO, PIT_RATE);
	io_outb(PIT_CH0_CMD, freq);
	io_outb(PIT_CH0_CMD, freq >> 8);

	irq_register(IRQ_PIT, &pit_handler);
}

void sleep(const u64 delay)
{
	done = false;
	target = delay;

	while (!done);
}
