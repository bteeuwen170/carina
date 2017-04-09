/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/8259.c
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
#include <asm/8259.h>

#include <kernel.h>

static const char devname[] = "pic";

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

	dprintf(devname, KP_NOTICE "remapped\n");
}

void pic_disable(void)
{
	io_outb(PIC_M_IO, 0xFF);
	io_outb(PIC_S_IO, 0xFF);

	dprintf(devname, KP_NOTICE "disabled\n");
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

int irq_active(u8 irq)
{
	(void) irq;
	/* io_outb(PIC_M_CMD, 0x0B);
	io_outb(PIC_S_CMD, 0x0B);
	return (io_inb(PIC_S_CMD) << 8 | io_inb(PIC_M_CMD)); */

	return 1;
}
