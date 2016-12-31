/*
 *
 * Elarix
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

#include <asm/cpu.h>

#include <kernel.h>

#include "pit.h"

/* TODO Should store as 10ms */
u64 ticks;

time_t uptime(void)
{
#ifdef ARCH_i386
	/* XXX TEMP XXX */
	return ((u32) ticks) / 1000;
#endif
#ifdef ARCH_x86_64
	return ticks / 1000;
#endif
}

static int int_handler(struct int_stack *regs)
{
	(void) regs;

	ticks++;

	return 1;
}

void sleep(const u64 delay)
{
	u64 target = ticks + delay;

	while (ticks < target)
		asm volatile ("hlt");
}

int timer_init(void)
{
	int res;
	u32 val;

	res = irq_handler_reg(IRQ_PIT, &int_handler);

	if (res < 0)
		return res;

	/* val = PIT_FREQ / 100; */
	val = PIT_FREQ / 1000;
	ticks = 0;

	io_outb(PIT_CMD, PIT_RATE);
	io_outb(PIT_CH0_IO, val & 0xFF);
	io_outb(PIT_CH0_IO, (val >> 8) & 0xFF);

	return res;
}
