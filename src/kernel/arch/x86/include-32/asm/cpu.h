/*
 *
 * Elarix
 * src/kernel/arch/x86/include-32/asm/cpub.h
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

struct segment_desc {
	u16	limit_lo;
	u8	base_lo[3];
	u16	flags;
	u8	base_hi;
} __attribute__ ((packed));

struct desc_register {
	u16	limit;
	void	*base;
} __attribute__ ((packed));

struct idt_desc {
	u16	offset_lo;
	u16	segment;
	u8	zero;
	u8	flags;
	u16	offset_hi;
} __attribute__ ((packed));

struct int_stack {
	u16	ds;
	u16	reserved0;
	u32	eax, ecx, edx, ebx, ebp, esi, edi;
	u32	int_no;
	u32	err_code;
	u32	eip;
	u16	cs;
	u16	reserved2;
	u32	eflags;
	u32	esp;
	u16	ss;
	u16	reserved3;
} __attribute__ ((packed));

struct tss_entry {
	u32	reserved0;
	u64	rsp0, rsp1, rsp2;
	u64	reserved1;
	u64	ist1, ist2, ist3, ist4, ist5, ist6, ist7;
	u16	reserved2[5];
	u16	iomap_base;
} __attribute__ ((packed));
