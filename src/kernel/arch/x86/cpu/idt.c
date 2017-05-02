/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/idt.c
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

#include <kernel.h>

#include <asm/cpu.h>
#include <asm/int.h>
#include <asm/pic.h>

static const char devname[] = "idt";

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
static int (*isr_handlers[IDT_ENTRIES]) (struct int_stack *regs);

/*
 * Valid values for type are:
 * - Interrupt Gate	(0b1110)
 * - Task Gate		(0b1001)
 */
/* TODO Add parameter for privilege level */
static void idt_set(const u8 gate, const u8 type, const uintptr_t offset)
{
	idt[gate].offset_lo	= offset & 0xFFFF;
	idt[gate].segment	= 0x08;
	idt[gate].zero		= 0;
	idt[gate].flags		= type | 0b10000000;
#ifdef ARCH_i386
	idt[gate].offset_hi	= (offset >> 16) & 0xFFFF;
#endif
#ifdef ARCH_x86_64
	idt[gate].offset_hi[0]	= (offset >> 16) & 0xFFFF;
	idt[gate].offset_hi[1]	= (offset >> 32) & 0xFFFF;
	idt[gate].offset_hi[2]	= (offset >> 48) & 0xFFFF;
#endif
}

void idt_init(void)
{
	int i;

	for (i = 0; i < IDT_ENTRIES; i++) {
		idt_set(i, 0x0E, (uintptr_t) ints[i]);

		if (i > SINT_ENTRIES && i < SINT_ENTRIES + IRQ_ENTRIES - 1)
			irq_mask(SINT_ENTRIES - i);
	}

	idt_load(&idt, IDT_ENTRIES * sizeof(struct idt_desc) - 1);
}

void _isr(struct int_stack *regs)
{
	if (regs->int_no < SINT_ENTRIES) {
#ifdef ARCH_i386
		panic(exceptions[regs->int_no],
				regs->err_code, regs->eip);
#endif
#ifdef ARCH_x86_64
		panic(exceptions[regs->int_no],
				regs->err_code, regs->rip);
#endif
	}

	if (isr_handlers[regs->int_no])
		isr_handlers[regs->int_no](regs);

	/* TODO Handle spurious interrupts */
	if (regs->int_no < SINT_ENTRIES + IRQ_ENTRIES - 1)
		pic_eoi(regs->int_no);
}

int isr_handler_reg(const u8 int_no, int (*handler) (struct int_stack *))
{
	if (isr_handlers[int_no])
		return -1;

	isr_handlers[int_no] = handler;

	if (int_no >= SINT_ENTRIES && int_no < SINT_ENTRIES + IRQ_ENTRIES - 1)
		irq_unmask(SINT_ENTRIES - int_no);

	return 0;
}

void isr_handler_unreg(const u8 int_no)
{
	if (int_no < SINT_ENTRIES)
		dprintf(KP_WARN "removing exception handler for int. %d\n",
				int_no);

	isr_handlers[int_no] = NULL;
}
