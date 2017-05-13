/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/pic.c
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
#include <asm/pic.h>

#define PIC_M_CMD	0x20
#define PIC_M_IO	0x21
#define PIC_M_OFF	0x20

#define PIC_S_CMD	0xA0
#define PIC_S_IO	0xA1
#define PIC_S_OFF	0x28

#define PIC_EOI		0x20

#define ICW1_PIC_M	0x02
#define ICW1_PIC_S	0x04

#define ICW1_INIT	0x11
#define ICW1_8086	0x01

void irq_mask(u8 irq)
{
	u16 port;

	if (irq < 8)
		port = PIC_M_IO;
	else
		port = PIC_S_IO;

	io_outb(port, io_inb(port) & (1 << (irq % 8)));
}

void irq_unmask(u8 irq)
{
	u16 port;

	if (irq < 8)
		port = PIC_M_IO;
	else
		port = PIC_S_IO;

	io_outb(port, io_inb(port) & ~(1 << (irq % 8)));
}

void pic_eoi(u8 int_no)
{
	if (int_no >= SINT_ENTRIES + (IRQ_ENTRIES / 2))
		io_outb(PIC_S_CMD, PIC_EOI);
	io_outb(PIC_M_CMD, PIC_EOI);
}

void pic_init(void)
{
	io_outb(PIC_M_CMD, ICW1_INIT);
	io_outb(PIC_S_CMD, ICW1_INIT);

	io_outb(PIC_M_IO, PIC_M_OFF);
	io_outb(PIC_S_IO, PIC_S_OFF);

	io_outb(PIC_M_IO, ICW1_PIC_S);
	io_outb(PIC_S_IO, ICW1_PIC_M);

	io_outb(PIC_M_IO, ICW1_8086);
	io_outb(PIC_S_IO, ICW1_8086);
}
