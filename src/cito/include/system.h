/*
 *
 * Carina
 * Header for System
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

      /**************************!!!REVISE!!!**************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <stdlib.h>

/* CPUID */
static inline void cpuid(u32 code, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile ("cpuid" :
				  "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) :
				  "0" (code));
}


/* Interrupts */
#define IDT_ENTRIES		256
#define ISR_ENTRIES		32
#define IRQ_ENTRIES		16

typedef struct {
	u16 ba_low;
	u16 ss;
	u8 zero;
	u8 flags;
	u16 ba_mid;
	u32 ba_high;
	u32 reserved;
} __attribute__ ((packed)) idt_t;

typedef struct {
	u64 ds;
	u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
	u64 r8, r9, r10, r11, r12, r13, r14, r15;
	u64 int_no, err_code;
	u64 rip, cs, rflags, rsp, ss;
} registers_t;

enum IRQS {
	IRQ_PIT				= 0x00,
	IRQ_ACPI			= 0x09,
	IRQ_KBD				= 0x01,
	IRQ_MOUSE			= 0x0C,
	IRQ_COM0			= 0x04,
	IRQ_COM1			= 0x03,
	IRQ_ATA0			= 0x0E,
	IRQ_ATA1			= 0x0F
};

i8 idt_init(void);

void idt_set(const u8 gate, const u64 ba, const u8 type);

void irq_register(const u16 irq, void (*handler) (registers_t *regs));
void irq_deregister(const u16 irq);

static inline void idt_load(void *idt_ptr, u16 limit)
{
	struct {
		u16 limit;
		void *base;
	} __attribute__ ((packed)) _idt_ptr = {
		limit,
		idt_ptr
	};

	asm volatile ("lidt %0" : : "m"(_idt_ptr));
}

static inline bool int_state(void)
{
	register u64 rflags = 0; //TODO Use registers_t

	asm volatile ("pushf; popq %0" : "=r" (rflags));

	return (rflags & (1 << 9));
}

/* Local APIC */

i8 lapic_init();

i8 lapic_timer_init();

static inline u64 msr_in(u32 msr)
{
	register u32 lo, hi;

	asm volatile ("rdmsr" : "=d" (hi), "=a" (lo) : "c" (msr));

	return (hi << 31) | lo;
}

static inline void msr_out(u32 msr, u64 value)
{
	asm volatile ("wrmsr" ::
			"c" (msr), "d" ((u64) value >> 31), "a" ((u64) value));
}


/* IO */
static inline u8 io_inb(u16 port)
{
	u8 value;

	asm volatile ("inb %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

static inline u16 io_inw(u16 port)
{
	u16 value;

	asm volatile ("inw %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

static inline void io_outb(u16 port, u8 value)
{
	asm volatile ("outb %0, %1" : : "a" (value), "dN" (port));
}

static inline void io_outw(u16 port, u16 value)
{
	asm volatile ("outw %0, %1" : : "a" (value), "dN" (port));
}


/* PC Speaker */ //TODO Seperate
void pcspk_play(const u16 freq);

void pcspk_stop(void);


/* Power Management */
void reboot(void);


/* Programable Interrupt Controller */
#define PIC_M_CMD		0x20
#define PIC_M_IO		0x21
#define PIC_M_OFF		0x20

#define PIC_S_CMD		0xA0
#define PIC_S_IO		0xA1
#define PIC_S_OFF		0x28

#define PIC_EOI			0x20

#define ICW1_PIC_S		0x04
#define ICW1_PIC_M		0x02

#define ICW1_INIT		0x11
#define ICW1_8086		0x01


/* Programmable Interrupt Timer */ //TODO Seperate and in pcspkr.h
#define PIT_RATE		0b00110110 //XXX Or 00110100?
#define PIT_SPKR		0b10110110

#define PIT_FREQ		1193182

#define PIT_CH0_CMD		0x40
#define PIT_CH2_CMD		0x42
#define PIT_IO			0x43

u64 uptime(void);

void pit_init(void);

void sleep(const u64 delay);


/* Serial Interface */
#define COM0			0x3F8
#define COM1			0x2F8
#define COM2			0x3E8
#define COM3			0x2E8

void serial_init(const u16 port);

u16 serial_in(const u16 port);

void serial_out(const u16 port, const u8 value);


/* Software Multitasking */
void svmode_enter(void);

void usrmode_enter(void);

#endif
