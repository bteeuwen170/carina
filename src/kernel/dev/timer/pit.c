/*
 *
 * Elarix
 * src/kernel/dev/timer/pit.c
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

#include <dev.h>
#include <fs.h>
#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

static const char devname[] = "pit";

#define PIT_RATE	0b00110110
#define PIT_SPKR	0b10110110

#define PIT_FREQ	0x1234DE

#define PIT_CMD		0x43
#define PIT_CH0_IO	0x40
#define PIT_CH2_IO	0x42

volatile u64 ticks;

static int int_handler(struct int_stack *regs)
{
	(void) regs;

	ticks++;

	return 1;
}

static int pit_read(struct file *fp, char *buf, off_t off, size_t n)
{
	(void) fp, (void) off;

	*((u64 *) buf) = ticks;

	return n;
}

static struct file_ops pit_file_ops = {
	.read = &pit_read
};

static int pit_probe(struct device *devp)
{
	u32 freq;
	int res;

	if ((res = irq_handler_reg(IRQ_PIT, &int_handler)) < 0)
		return res;

	ticks = 0;
	freq = PIT_FREQ / 1000;

	io_outb(PIT_CMD, PIT_RATE);
	io_outb(PIT_CH0_IO, freq & 0xFF);
	io_outb(PIT_CH0_IO, (freq >> 8) & 0xFF);

	devp->op = &pit_file_ops;

	return 0;
}

static void pit_fini(struct device *devp)
{
	(void) devp;
}

static struct driver pit_driver = {
	.name	= devname,
	.major	= MAJOR_TMR,

	.busid	= BUS_NONE,
	.bus	= NULL,

	.probe	= &pit_probe,
	.fini	= &pit_fini
};

int pit_init(void)
{
	int res;

	if ((res = driver_reg(&pit_driver)) < 0)
		return res;

	return device_reg(&pit_driver, NULL, 0);
}

void pit_exit(void)
{
	driver_unreg(&pit_driver);
}

MODULE(pit, &pit_init, &pit_exit);
