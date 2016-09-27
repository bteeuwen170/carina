/*
 *
 * Carina
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

#include <stdio.h>
#include <string.h>
#include <cpu.h>
#include <system.h>
#include <kernel/print.h>
#include <8259.h>

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

static char *exceptions[ISR_ENTRIES] = {
	"Divide Error Exception (0x00)",
	"Debug Exception (0x01)",
	"NMI Interrupt (0x02)",
	"Breakpoint Exception (0x03)",
	"Overflow Exception (0x04)",
	"BOUND Range Exceeded Exception (0x05)",
	"Invalid Opcode Exception (0x06)",
	"Device Not Available Exception (0x07)",
	"Double Fault Exception (0x08)",
	"Coprocessor Segment Overrun (0x09)",
	"Invalid TSS Exception (0x0A)",
	"Segment Not Present (0x0B)",
	"Stack Fault Exception (0x0C)",
	"General Protection Exception (0x0D)",
	"Page-Fault Exception (0x0E)",
	"Reserved (0x0F)",
	"x87 FPU Floating-Point Error (0x10)",
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

/* TODO This is not very elegant */
extern void _int0();
extern void _int1();
extern void _int2();
extern void _int3();
extern void _int4();
extern void _int5();
extern void _int6();
extern void _int7();
extern void _int8();
extern void _int9();
extern void _int10();
extern void _int11();
extern void _int12();
extern void _int13();
extern void _int14();
extern void _int15();
extern void _int16();
extern void _int17();
extern void _int18();
extern void _int19();
extern void _int20();
extern void _int21();
extern void _int22();
extern void _int23();
extern void _int24();
extern void _int25();
extern void _int26();
extern void _int27();
extern void _int28();
extern void _int29();
extern void _int30();
extern void _int31();

extern void _int32();
extern void _int33();
extern void _int34();
extern void _int35();
extern void _int36();
extern void _int37();
extern void _int38();
extern void _int39();
extern void _int40();
extern void _int41();
extern void _int42();
extern void _int43();
extern void _int44();
extern void _int45();
extern void _int46();
extern void _int47();
/* TODO MORE!!! */

/* More entries */

/*
 * Valid values for type are:
 * - Interrupt Gate	(0b1110)
 * - Task Gate		(0b1001)
 */
/* TODO Add parameter for privilege level */
static void idt_set(const u8 gate, const u8 type, const u64 offset)
{
	idt[gate].offset_lo		= offset & 0xFFFF;
	idt[gate].segment		= 0x08;
	idt[gate].zero			= 0;
	idt[gate].flags			= type | 0b10000000;
	idt[gate].offset_hi[0]	= (offset >> 16) & 0xFFFF;
	idt[gate].offset_hi[1]	= (offset >> 32) & 0xFFFF;
	idt[gate].offset_hi[2]	= (offset >> 48) & 0xFFFF;
}

void idt_init(void)
{
	/* Nonmaskable interrupts */
	idt_set(0, 0x0E, (unsigned long) &_int0);
	idt_set(1, 0x0E, (unsigned long) &_int1);
	idt_set(2, 0x0E, (unsigned long) &_int2);
	idt_set(3, 0x0E, (unsigned long) &_int3);
	idt_set(4, 0x0E, (unsigned long) &_int4);
	idt_set(5, 0x0E, (unsigned long) &_int5);
	idt_set(6, 0x0E, (unsigned long) &_int6);
	idt_set(7, 0x0E, (unsigned long) &_int7);
	idt_set(8, 0x0E, (unsigned long) &_int8);
	idt_set(9, 0x0E, (unsigned long) &_int9);
	idt_set(10, 0x0E, (unsigned long) &_int10);
	idt_set(11, 0x0E, (unsigned long) &_int11);
	idt_set(12, 0x0E, (unsigned long) &_int12);
	idt_set(13, 0x0E, (unsigned long) &_int13);
	idt_set(14, 0x0E, (unsigned long) &_int14);
	idt_set(15, 0x0E, (unsigned long) &_int15);
	idt_set(16, 0x0E, (unsigned long) &_int16);
	idt_set(17, 0x0E, (unsigned long) &_int17);
	idt_set(18, 0x0E, (unsigned long) &_int18);
	idt_set(19, 0x0E, (unsigned long) &_int19);
	idt_set(20, 0x0E, (unsigned long) &_int20);
	idt_set(21, 0x0E, (unsigned long) &_int21);
	idt_set(22, 0x0E, (unsigned long) &_int22);
	idt_set(23, 0x0E, (unsigned long) &_int23);
	idt_set(24, 0x0E, (unsigned long) &_int24);
	idt_set(25, 0x0E, (unsigned long) &_int25);
	idt_set(26, 0x0E, (unsigned long) &_int26);
	idt_set(27, 0x0E, (unsigned long) &_int27);
	idt_set(28, 0x0E, (unsigned long) &_int28);
	idt_set(29, 0x0E, (unsigned long) &_int29);
	idt_set(30, 0x0E, (unsigned long) &_int30);
	idt_set(31, 0x0E, (unsigned long) &_int31);

	/* Master PIC */
	idt_set(32, 0x0E, (unsigned long) &_int32); irq_mask(IRQ_PIT);
	idt_set(33, 0x0E, (unsigned long) &_int33); irq_mask(IRQ_KBD);
	idt_set(34, 0x0E, (unsigned long) &_int34); irq_mask(IRQ_CAS);
	idt_set(35, 0x0E, (unsigned long) &_int35); irq_mask(IRQ_COM1);
	idt_set(36, 0x0E, (unsigned long) &_int36); irq_mask(IRQ_COM0);
	idt_set(37, 0x0E, (unsigned long) &_int37); irq_mask(IRQ_SND);
	idt_set(38, 0x0E, (unsigned long) &_int38); irq_mask(IRQ_FDD);
	idt_set(39, 0x0E, (unsigned long) &_int39); irq_mask(IRQ_PRT);

	/* Slave PIC */
	idt_set(40, 0x0E, (unsigned long) &_int40); irq_mask(IRQ_RTC);
	idt_set(41, 0x0E, (unsigned long) &_int41); irq_mask(IRQ_ACPI);
	idt_set(42, 0x0E, (unsigned long) &_int42); irq_mask(IRQ_OPT0);
	idt_set(43, 0x0E, (unsigned long) &_int43); irq_mask(IRQ_OPT1);
	idt_set(44, 0x0E, (unsigned long) &_int44); irq_mask(IRQ_PS2);
	idt_set(45, 0x0E, (unsigned long) &_int45); irq_mask(IRQ_FPU);
	idt_set(46, 0x0E, (unsigned long) &_int46); irq_mask(IRQ_ATA0);
	idt_set(47, 0x0E, (unsigned long) &_int47); irq_mask(IRQ_ATA1);

	idt_load(&idt, IDT_ENTRIES * sizeof(struct idt_desc) - 1);

	kprintf(KP_INFO, devname,
			"%d entries entered (FIXME hardcoded lies)\n", IDT_ENTRIES);
}

void _isr(struct int_stack *regs)
{
	switch (regs->int_no) {
	case ISR_ENTRIES + IRQ_PRT - 1:
	case ISR_ENTRIES + IRQ_ATA1 - 1:
		break;
	default:
		if (regs->int_no < ISR_ENTRIES) { //TODO Don't always halt
			panic(exceptions[regs->int_no], regs->err_code);
		} else if (regs->int_no < ISR_ENTRIES + IRQ_ENTRIES) {
			void (*handler) (struct int_stack *regs) =
					irq_handlers[regs->int_no - ISR_ENTRIES];

			if (handler)
				handler(regs);

			if (regs->int_no >= ISR_ENTRIES + (IRQ_ENTRIES / 2))
				io_outc(PIC_S_CMD, PIC_EOI);
			io_outc(PIC_M_CMD, PIC_EOI);
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
