/*
 *
 * Carina
 * src/kernel/arch/x86/cpu/8259.c
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

#include <cpu.h>
#include <8259.h>
#include <kernel/print.h>

static char *devname = "pic";

void pic_remap(void)
{
	io_outb(PIC_M_CMD, ICW1_INIT);
	io_outb(PIC_S_CMD, ICW1_INIT);

	io_outb(PIC_M_IO, PIC_M_OFF);
	io_outb(PIC_S_IO, PIC_S_OFF);

	io_outb(PIC_M_IO, ICW1_PIC_S);
	io_outb(PIC_S_IO, ICW1_PIC_M);

	io_outb(PIC_M_IO, ICW1_8086);
	io_outb(PIC_S_IO, ICW1_8086);

	kprintf(KP_INFO, devname, "remapped\n");
}

void pic_disable(void)
{
	io_outb(PIC_M_IO, 0xFF);
	io_outb(PIC_S_IO, 0xFF);

	kprintf(KP_INFO, devname, "disabled\n");
}

void irq_mask(u8 irq)
{
	u16 port;
	
	if (irq < IRQ_ENTRIES / 2) {
		port = PIC_M_IO;
	} else {
		port = PIC_S_IO;
		irq -= IRQ_ENTRIES / 2;
	}

	io_outb(port, io_inb(port) | (1 << irq));
}

void irq_unmask(u8 irq)
{
	u16 port;
	
	if (irq < IRQ_ENTRIES / 2) {
		port = PIC_M_IO;
	} else {
		port = PIC_S_IO;
		irq -= IRQ_ENTRIES / 2;
	}

	io_outb(port, io_inb(port) & ~(1 << irq));
}
