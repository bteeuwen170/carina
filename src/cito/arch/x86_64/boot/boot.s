/*
 *
 * Carina
 * Main Cito Kernel Bootstrap
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

.set VMA,			0xFFFFFFFF80000000


.section .mboot

.set MB_MAGIC,		0x1BADB002
.set MB_FLAGS,		0x10003

mboot_header:
	.long MB_MAGIC
	.long MB_FLAGS
	.long -(MB_MAGIC + MB_FLAGS)
	.long (mboot_header - VMA)
	.long 0x100000
	.long (bss - VMA)
	.long (end - VMA)
	.long (bootstrap - VMA)


.section .text

.code32
bootstrap:
	movl %ebx, %edi

	movl %cr4, %eax
	bts $5, %eax
	movl %eax, %cr4

	movl $(pml4 - VMA), %eax
	movl %eax, %cr3

	movl $0xc0000080, %ecx
	rdmsr
	bts $8, %eax
	wrmsr

	movl %cr0, %eax
	bts $31, %eax
	movl %eax, %cr0

	lgdt (gdt_ptr - VMA)
	ljmp $0x08, $(start - VMA)

.code64
start:
	cli

	movq $(end), %rsp

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	xor %ax, %ax
	mov %ax, %ss

	call kernel_main

	cli

halt:
	hlt
	jmp halt


.code32


.section .data

.align 0x1000
gdt:
	.quad 0x0000000000000000
	.quad 0x00AF9A000000FFFF	/* 0x08 - Code Kernel */
	.quad 0x00CF92000000FFFF	/* 0x10 - Data Kernel */
	.quad 0x00CFF2000000FFFF	/* 0x18 - Code Userspace */
	.quad 0x00AFFA000000FFFF	/* 0x20 - Data Userspace */ 
gdt_ptr:
	.word . - gdt - 1		/* Limit */
	.quad gdt - VMA			/* Base */


.section .padata

.align 0x1000
pml4:
	.quad (pml3a - VMA + 0x03)
	.fill 255, 8, 0
	.quad (pml4 - VMA + 0x03)
	.fill 254, 8, 0
	.quad (pml3b - VMA + 0x03)
pml3a:
	.quad (pml2 - VMA + 0x03)
	.fill 511, 8, 0
pml3b:
	.fill 510, 8, 0
	.quad (pml2 - VMA + 0x03)
	.quad 0x00
pml2:
	.quad (pml1 - VMA + 0x03)
	.fill 511, 8, 0
pml1:
	i = 0x00
	.rept 0x200
	.quad (i + 0x03)
	i = i + 0x1000
	.endr
