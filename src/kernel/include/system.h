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

#include <cpu.h> //XXX TEMP XXX

/* CPUID */
static inline void cpuid(u32 code, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile ("cpuid" :
				  "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) :
				  "0" (code));
}


/* Interrupts */

enum IRQS {
	/* Master PIC */
	IRQ_PIT				= 0x00,
	IRQ_KBD				= 0x01,
	IRQ_CAS				= 0x02,
	IRQ_COM1			= 0x03,
	IRQ_COM0			= 0x04,
	IRQ_SND				= 0x05,
	IRQ_FDD				= 0x06,
	IRQ_PRT				= 0x07,
	/* Slave PIC */
	IRQ_RTC				= 0x08,
	IRQ_ACPI			= 0x09,
	IRQ_OPT0			= 0x0A,
	IRQ_OPT1			= 0x0B,
	IRQ_PS2				= 0x0C,
	IRQ_FPU				= 0x0D,
	IRQ_ATA0			= 0x0E,
	IRQ_ATA1			= 0x0F,
};

void idt_init(void);

void irq_reghandler(const u8 irq, void (*handler) (struct int_stack *regs));
void irq_unreghandler(const u8 irq);

void irq_mask(u8 irq);
void irq_unmask(u8 irq);

static inline void idt_load(void *base, u16 limit)
{
	struct desc_register idt_ptr = {
		limit,
		base
	};

	asm volatile ("lidt %0" : : "m"(idt_ptr));
}

static inline void gdt_load(void *base, u16 limit)
{
	struct desc_register gdt_ptr = {
		limit,
		base
	};

	asm volatile ("lgdt %0" : : "m"(gdt_ptr));
}

void tss_init(u32 *limit, u32 *base);

static inline bool int_state(void)
{
	register u64 rflags = 0;

	asm volatile ("pushf; popq %0" : "=r" (rflags));

	return (rflags & (1 << 9));
}


/* IO */
static inline u8 io_inc(u16 port)
{
	u8 val;

	asm volatile ("inb %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline u16 io_ins(u16 port)
{
	u16 val;

	asm volatile ("inw %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline u32 io_ini(u16 port)
{
	u32 val;

	asm volatile ("inl %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline void io_outc(u16 port, u8 val)
{
	asm volatile ("outb %0, %1" : : "a" (val), "dN" (port));
}

static inline void io_outs(u16 port, u16 val)
{
	asm volatile ("outw %0, %1" : : "a" (val), "dN" (port));
}

static inline void io_outi(u16 port, u32 val)
{
	asm volatile ("outl %0, %1" : : "a" (val), "dN" (port));
}


/* PC Speaker */
void pcspk_play(const u16 freq);

void pcspk_stop(void);

void pcspk_fj(void); /* Frere Jacques */
void pcspk_mi(void); /* The Secret Of Monkey Island intro */
void pcspk_hc(void); /* Eagles - Hotel California */
void pcspk_acri(void); /* Bob Acri - Sleep Away */


/* Power Management */
void reboot(void);


/* Programmable Interrupt Timer */
#define PIT_RATE		0b00110110
#define PIT_SPKR		0b10110110

#define PIT_FREQ		0x1234DE

#define PIT_IO			0x43
#define PIT_CH0_CMD		0x40
#define PIT_CH2_CMD		0x42
#define PIT_CH2_IO		0x61

u32 uptime(void);

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
