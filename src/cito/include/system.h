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

#include <stdint.h>

/* CMOS */
#define CMOS_CMD		0x70
#define CMOS_IO			0x71

enum CMOS_REGISTERS {
	CMOS_CENTURY		= 0x32,
	CMOS_YEAR			= 0x09,
	CMOS_MONTH			= 0x08,
	CMOS_DAY			= 0x07,
	CMOS_HOURS			= 0x04,
	CMOS_MINUTES		= 0x02,
	CMOS_SECONDS		= 0x00,
	CMOS_A				= 0x0A,
	CMOS_B				= 0x0B,
	CMOS_C				= 0x0C,
	CMOS_D				= 0x0D,
	CMOS_CONFIG			= 0x11,
	CMOS_DIAG			= 0x0E,
	CMOS_PERIF			= 0x14
};

uint8_t cmos_in(const uint8_t reg);


/* CPUID */
static inline void cpuid(uint32_t code, uint32_t *eax, uint32_t *edx)
{
	asm volatile ("cpuid" :
				  "=a" (*eax), "=d" (*edx) :
				  "0" (code) :
				  "ebx", "ecx");
}


/* Interrupts */
#define IDT_ENTRIES		256
#define ISR_ENTRIES		32
#define IRQ_ENTRIES		16

typedef struct {
	uint16_t ba_low;
	uint16_t ss;
	uint8_t zero;
	uint8_t flags;
	uint16_t ba_mid;
	uint32_t ba_high;
	uint32_t reserved;
} __attribute__ ((packed)) idt_t;

typedef struct {
	uint64_t ds;
	uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
	uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
	uint64_t int_no, err_code;
	uint64_t rip, cs, rflags, rsp, ss;
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

void idt_init(void);

void idt_set(const uint8_t gate, const uint64_t ba, const uint8_t type);

void irq_register(const uint16_t irq, void (*handler) (registers_t *regs));
void irq_deregister(const uint16_t irq);

static inline void idt_load(void *idt_ptr, uint16_t limit)
{
	struct {
		uint16_t limit;
		void *base;
	} __attribute__ ((packed)) _idt_ptr = {
		limit,
		idt_ptr
	};

	asm volatile ("lidt %0" : : "m"(_idt_ptr));
}


/* IO */
static inline uint8_t io_inb(uint16_t port)
{
	uint8_t value;

	asm volatile ("inb %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

static inline uint16_t io_inw(uint16_t port)
{
	uint16_t value;

	asm volatile ("inw %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

static inline void io_outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %0, %1" : : "a" (value), "dN" (port));
}

static inline void io_outw(uint16_t port, uint16_t value)
{
	asm volatile ("outw %0, %1" : : "a" (value), "dN" (port));
}


/* PC Speaker */
void pcspk_play(const uint16_t freq);

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


/* Programmable Interrupt Timer */
#define PIT_CH0_CMD		0x40
#define PIT_CH1_CMD		0x41
#define PIT_CH2_CMD		0x42
#define PIT_IO			0x43

uint64_t uptime(void);

void pit_init();

void sleep(const uint64_t delay);


/* Serial Interface */
#define COM0			0x3F8
#define COM1			0x2F8
#define COM2			0x3E8
#define COM3			0x2E8

void serial_init(const uint16_t port);

uint16_t serial_in(const uint16_t port);

void serial_out(const uint16_t port, const uint8_t value);


/* Software Multitasking */
void svmode_enter(void);

void usrmode_enter(void);

#endif
