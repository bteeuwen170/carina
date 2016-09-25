/*
 *
 * Carina
 * src/kernel/dev/timer/pit.c
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


#include <system.h>
#include <div64.h>
#include <kernel/print.h>

/* TODO Should store as 10ms */
u64 ticks;

u64 uptime(void)
{
	return ticks / 1000;
	//return do_div(ticks, 1000);
}

static void int_handler(struct int_stack *regs)
{
	(void) regs;
	ticks++;
}

void pit_init(void)
{
	u32 val;

	//val = PIT_FREQ / 100;
	val = PIT_FREQ / 1000;
	ticks = 0;

	io_outc(PIT_IO, PIT_RATE);
	io_outc(PIT_CH0_CMD, val & 0xFF);
	io_outc(PIT_CH0_CMD, (val >> 8) & 0xFF);

	irq_reghandler(IRQ_PIT, &int_handler);
}

void sleep(const u64 delay)
{
	u64 target = ticks + delay;

	while (ticks < target)
		asm volatile ("hlt");
}