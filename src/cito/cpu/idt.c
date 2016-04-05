/*
 *
 * Carina
 * IDT, ISR and IRQ configuration
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>

static string exceptions[ISR_ENTRIES] = {
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
	"Security Exception (0x1E)",
	"Reserved (0x1F)",
};

static idt_t idt[IDT_ENTRIES];
void *irq_handlers[IRQ_ENTRIES];

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

i8 idt_init(void)
{
	io_outb(PIC_M_CMD, ICW1_INIT);
	io_outb(PIC_S_CMD, ICW1_INIT);

	io_outb(PIC_M_IO, PIC_M_OFF);
	io_outb(PIC_S_IO, PIC_S_OFF);

	io_outb(PIC_M_IO, ICW1_PIC_S);
	io_outb(PIC_S_IO, ICW1_PIC_M);

	io_outb(PIC_M_IO, ICW1_8086);
	io_outb(PIC_S_IO, ICW1_8086);

	io_outb(PIC_M_IO, 0x00);
	io_outb(PIC_S_IO, 0x00);

	memset(&idt, 0, IDT_ENTRIES * sizeof(idt_t));

	idt_set(0, (u64) & _isr0, 0x8E);
	idt_set(1, (u64) & _isr1, 0x8E);
	idt_set(2, (u64) & _isr2, 0x8E);
	idt_set(3, (u64) & _isr3, 0x8E);
	idt_set(4, (u64) & _isr4, 0x8E);
	idt_set(5, (u64) & _isr5, 0x8E);
	idt_set(6, (u64) & _isr6, 0x8E);
	idt_set(7, (u64) & _isr7, 0x8E);
	idt_set(8, (u64) & _isr8, 0x8E);
	idt_set(9, (u64) & _isr9, 0x8E);
	idt_set(10, (u64) & _isr10, 0x8E);
	idt_set(11, (u64) & _isr11, 0x8E);
	idt_set(12, (u64) & _isr12, 0x8E);
	idt_set(13, (u64) & _isr13, 0x8E);
	idt_set(14, (u64) & _isr14, 0x8E);
	idt_set(15, (u64) & _isr15, 0x8E);
	idt_set(16, (u64) & _isr16, 0x8E);
	idt_set(17, (u64) & _isr17, 0x8E);
	idt_set(18, (u64) & _isr18, 0x8E);
	idt_set(19, (u64) & _isr19, 0x8E);
	idt_set(20, (u64) & _isr20, 0x8E);
	idt_set(21, (u64) & _isr21, 0x8E);
	idt_set(22, (u64) & _isr22, 0x8E);
	idt_set(23, (u64) & _isr23, 0x8E);
	idt_set(24, (u64) & _isr24, 0x8E);
	idt_set(25, (u64) & _isr25, 0x8E);
	idt_set(26, (u64) & _isr26, 0x8E);
	idt_set(27, (u64) & _isr27, 0x8E);
	idt_set(28, (u64) & _isr28, 0x8E);
	idt_set(29, (u64) & _isr29, 0x8E);
	idt_set(30, (u64) & _isr30, 0x8E);
	idt_set(31, (u64) & _isr31, 0x8E);

	idt_set(32, (u64) & _irq0, 0x8E);
	idt_set(33, (u64) & _irq1, 0x8E);
	idt_set(34, (u64) & _irq2, 0x8E);
	idt_set(35, (u64) & _irq3, 0x8E);
	idt_set(36, (u64) & _irq4, 0x8E);
	idt_set(37, (u64) & _irq5, 0x8E);
	idt_set(38, (u64) & _irq6, 0x8E);
	idt_set(39, (u64) & _irq7, 0x8E);
	idt_set(40, (u64) & _irq8, 0x8E);
	idt_set(41, (u64) & _irq9, 0x8E);
	idt_set(42, (u64) & _irq10, 0x8E);
	idt_set(43, (u64) & _irq11, 0x8E);
	idt_set(44, (u64) & _irq12, 0x8E);
	idt_set(45, (u64) & _irq13, 0x8E);
	idt_set(46, (u64) & _irq14, 0x8E);
	idt_set(47, (u64) & _irq15, 0x8E);

	idt_load(&idt, IDT_ENTRIES * sizeof(idt_t) - 1);

	memset(irq_handlers, 0, IRQ_ENTRIES * sizeof(void)); //TODO Typedef

	return OK;
}

void idt_set(const u8 gate, const u64 ba, const u8 flags)
{
	idt[gate].ba_low		= ba & 0xFFFF;
	idt[gate].ba_mid		= (ba >> 16) & 0xFFFF;
	idt[gate].ba_high		= (ba >> 32) & 0xFFFFFFFF;

	idt[gate].flags			= flags;
	idt[gate].ss			= 0x08;

	idt[gate].zero			= 0x00;
	idt[gate].reserved		= 0x00;
}

void _int_handler(registers_t *registers) //FIXME HACKS DETECTED
{
	//prints(itoa(registers->int_no, 10));
	switch (registers->int_no) {
		case ISR_ENTRIES + 6:
		case ISR_ENTRIES + 14:
			break;
		default:
			if (registers->int_no < ISR_ENTRIES) { //TODO Don't always halt
				panic(exceptions[registers->int_no], registers->err_code);
			} else if (registers->int_no < ISR_ENTRIES + IRQ_ENTRIES) {
				void (*handler) (registers_t *registers) =
						irq_handlers[registers->int_no - ISR_ENTRIES];

				if (handler) handler(registers);

				if (registers->int_no >= ISR_ENTRIES + (IRQ_ENTRIES / 2))
					io_outb(PIC_S_CMD, PIC_EOI);
				else
					io_outb(PIC_M_CMD, PIC_EOI);
			}
			break;
	}
}

//void int_start(

void irq_register(const u16 irq, void (*handler) (registers_t *registers))
{
	irq_handlers[irq] = handler;
}

void irq_deregister(const u16 irq)
{
	irq_handlers[irq] = 0x00;
}
