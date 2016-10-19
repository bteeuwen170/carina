/*
 *
 * Clemence
 * src/kernel/arch/x86/cpu/idt.c
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

#include <print.h>
#include <syscall.h>

#include <asm/8259.h>
#include <asm/cpu.h>
#include <asm/int.h>

#include <stdio.h>
#include <string.h>

static char *devname = "idt";

/*
 * This is a global list of all interrupt vectors:
 * 0x00 - 0x1F	- Nonmaskable interrupts
 * 0x20 - 0x2F	- IRQs
 * 0x20 - 0x7F	- Reserved
 * 0x80			- System calls
 * 0x81 - 0xFD	- Reserved
 * 0xFE	- 0xFF	- Unused
 */

static char *exceptions[SINT_ENTRIES] = {
	"Divide Error Exception (0x00)",
	"Debug Exception (0x01)",
	"NMI Interrupt (0x02)",
	"Breakpoint Exception (0x03)",
	"Overflow Exception (0x04)",
	"BOUND Range Exceeded Exception (0x05)",
	"Illegal instruction (0x06)",
	"Device Not Available Exception (0x07)",
	"Double Fault Exception (0x08)",
	"Reserved (0x09)",
	"Invalid TSS Exception (0x0A)",
	"Segment Not Present (0x0B)",
	"Stack Fault Exception (0x0C)",
	"General Protection Exception (0x0D)",
	"Page-Fault Exception (0x0E)",
	"Reserved (0x0F)",
	"Reserved (0x10)",
	"Alignment Check Exception (0x11)",
	"Machine-Check Exception (0x12)",
	"SIMD Floating-Point Exception (0x13)",
	"Virtualization Exception (0x14)",
	"Reserved (0x15)",
	"Reserved (0x16)",
	"Reserved (0x17)",
	"Reserved (0x18)",
	"Reserved (0x19)",
	"Reserved (0x1A)",
	"Reserved (0x1B)",
	"Reserved (0x1C)",
	"Reserved (0x1D)",
	"Reserved (0x1E)",
	"Reserved (0x1F)",
};

static struct idt_desc idt[IDT_ENTRIES];
void *irq_handlers[IRQ_ENTRIES];

/*
 * Valid values for type are:
 * - Interrupt Gate	(0b1110)
 * - Task Gate		(0b1001)
 */
/* TODO Add parameter for privilege level */
static void idt_set(const u8 gate, const u8 type, const u64 offset)
{
	idt[gate].offset_lo	= offset & 0xFFFF;
	idt[gate].segment	= 0x08;
	idt[gate].zero		= 0;
	idt[gate].flags		= type | 0b10000000;
	idt[gate].offset_hi[0]	= (offset >> 16) & 0xFFFF;
	idt[gate].offset_hi[1]	= (offset >> 32) & 0xFFFF;
	idt[gate].offset_hi[2]	= (offset >> 48) & 0xFFFF;
}

void idt_init(void)
{
	int i;

	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_set(i, 0x0E, (vaddr_t) ints[i]);

		if (i > SINT_ENTRIES - 1)
			irq_mask(SINT_ENTRIES - 1 + i);
	}

	idt_load(&idt, IDT_ENTRIES * sizeof(struct idt_desc) - 1);

	kprintf(KP_INFO, devname,
			"%d entries entered (FIXME hardcoded lies)\n", i);
}

void _isr(struct int_stack *regs)
{
	switch (regs->int_no) {
	case SINT_ENTRIES - 1 + IRQ_PRT:
	case SINT_ENTRIES - 1 + IRQ_ATA1:
		break;
	case 0x08: /* Double fault */
	case 0x12: /* Internal machine error */
		panic(exceptions[regs->int_no],
				regs->err_code, regs->rip);
		break;
	case SINT_SYSCALL:
		break;
	default:
		if (regs->int_no < SINT_ENTRIES) { //XXX TEMPORARY
			panic(exceptions[regs->int_no],
					regs->err_code, regs->rip);
		} else if (regs->int_no < SINT_ENTRIES + IRQ_ENTRIES) {
			void (*handler) (struct int_stack *regs);

			handler = irq_handlers[regs->int_no - SINT_ENTRIES];

			if (handler)
				handler(regs);

			if (regs->int_no >= SINT_ENTRIES + (IRQ_ENTRIES / 2))
				io_outb(PIC_S_CMD, PIC_EOI);
			io_outb(PIC_M_CMD, PIC_EOI);
		}

		break;
	}
}

void irq_reghandler(const u8 irq, void (*handler) (struct int_stack *))
{
	irq_handlers[irq] = handler;
	irq_unmask(irq);
}

void irq_unreghandler(const u8 irq)
{
	irq_handlers[irq] = 0;
}
