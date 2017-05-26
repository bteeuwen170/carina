/*
 *
 * Elarix
 * src/kernel/arch/x86/include/asm/cpu.h
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

#ifndef _X86_CPU_H
#define _X86_CPU_H

#ifdef ARCH_i386
#   include "../../include-32/asm/cpu.h"
#endif
#ifdef ARCH_x86_64
#   include "../../include-64/asm/cpu.h"
#endif

#define PAGE_SIZE	4096

#define GDT_ENTRIES	6

#define IDT_ENTRIES	256
#define SINT_ENTRIES	32
#define IRQ_ENTRIES	16

#define TSS_ENTRIES	1

#define IRQ_PIT		0x00
#define IRQ_KBD		0x01
#define IRQ_CAS		0x02
#define IRQ_COM1	0x03
#define IRQ_COM0	0x04
#define IRQ_SND		0x05
#define IRQ_FDD		0x06
#define IRQ_PRT		0x07
#define IRQ_RTC		0x08
#define IRQ_ACPI	0x09
#define IRQ_OPT0	0x0A
#define IRQ_OPT1	0x0B
#define IRQ_PS2		0x0C
#define IRQ_FPU		0x0D
#define IRQ_ATA0	0x0E
#define IRQ_ATA1	0x0F

static inline void cpuid(u32 code, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx)
{
	asm volatile ("cpuid" :
			"=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) :
			"0" (code));
}

static inline u8 io_inb(u16 port)
{
	u8 val;

	asm volatile ("inb %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline u16 io_inw(u16 port)
{
	u16 val;

	asm volatile ("inw %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline u32 io_ind(u16 port)
{
	u32 val;

	asm volatile ("inl %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

static inline void io_insw(u16 port, char *buf)
{
	asm volatile ("insw" : "+D" (buf) : "dN" (port) : "memory");
}

static inline void io_outb(u16 port, u8 val)
{
	asm volatile ("outb %0, %1" : : "a" (val), "dN" (port));
}

static inline void io_outw(u16 port, u16 val)
{
	asm volatile ("outw %0, %1" : : "a" (val), "dN" (port));
}

static inline void io_outd(u16 port, u32 val)
{
	asm volatile ("outl %0, %1" : : "a" (val), "dN" (port));
}

static inline void gdt_load(void *base, u16 limit)
{
	struct desc_register gdt_ptr = {
		limit,
		base
	};

	asm volatile ("lgdt %0" : : "m" (gdt_ptr));
}

static inline void idt_load(void *base, u16 limit)
{
	struct desc_register idt_ptr = {
		limit,
		base
	};

	asm volatile ("lidt %0" : : "m" (idt_ptr));
}

static inline void tss_load(void)
{
	register u16 ax = 0x2B;

	asm volatile ("ltr %0" : : "r" (ax));
}

static inline void tlb_flush(void)
{
#ifdef ARCH_x86_64
	asm volatile ("mov %0, %%cr3" : : "r" (0xFFFFFF7FBFDFE000) : "memory");
#else
	/* TODO */
#endif
}

static inline void page_flush(uintptr_t addr)
{
#if 1
	asm volatile ("invlpg (%0)" : : "r" (addr) : "memory");
#else /* For i386 */
	tlb_flush();
#endif
}

/*static inline bool int_state(void)
{
	register u64 rflags = 0;

	asm volatile ("pushf; popq %0" : "=r" (rflags));

	return (rflags & (1 << 9));
}*/

void idt_init(void);

int isr_handler_reg(const u8 int_no, int (*handler) (struct int_stack *regs));
void isr_handler_unreg(const u8 int_no);

#define irq_handler_reg(i, h)	isr_handler_reg(SINT_ENTRIES + i, h)
#define irq_handler_unreg(i)	isr_handler_unreg(SINT_ENTRIES + i)

void irq_mask(u8 irq);
void irq_unmask(u8 irq);

int irq_active(u8 irq);

/* void tss_init(u32 *limit, u32 *base);

void svmode_enter(void);
void usrmode_enter(void); */

uintptr_t page_get(uintptr_t page);

void *page_alloc_user(void);
void *page_alloc_kernel(void);

void page_free(void *page);

void pm_init(void);

#endif
