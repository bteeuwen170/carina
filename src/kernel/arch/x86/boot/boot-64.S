/*
 *
 * Clemence
 * src/kernel/arch/x86/boot/boot-64.S
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
 
.code32

.section .text

.global bootstrap
bootstrap:
	/* TODO Enable SSE and SSE2 */

	//call paging_init
	mov $pml4e, %eax
	//mov $0x200000, %eax
	mov %eax, %cr3

	/* Enable long mode */
	mov $0xC0000080, %ecx
	rdmsr
	bts $8, %eax /* IA32_EFER.LME */
	wrmsr

	/* Enable paging */
	mov %cr0, %eax
	bts $31, %eax /* CR0.PG */
	bts $16, %eax /* CR0.WP */
	mov %eax, %cr0

	call gdt_init

	ljmp $0x08, $start


.section .data

.align 4096
/* Controls 512 GB */
pml4e:
	.quad (pdpte + 3)
	.fill 510, 8, 0
	.quad (pdpte2 + 3)
/* Controls 1 GB */
pdpte:
	.quad (pde + 3)
	.fill 511, 8, 0
/* Controls 1 GB */
pdpte2:
	.fill 510, 8, 0
	.quad (pde + 3)
	.quad 0
/* Controls 2 MB */
pde:
	.quad (pte + 3)
	.quad (pte2 + 3)
	.fill 510, 8, 0
/* Controls 4 KB */
pte:
	i = 0x00
	.rept 512
		.quad (i + 3)
		i = i + 4096
	.endr
/* TEMP */
pte2:
	i = 0x200000
	.rept 512
		.quad (i + 3)
		i = i + 4096
	.endr


.code64

.section .text

start:
	/* Intel Software Developer's Manual V3 6.14.3 */
	xor %ax, %ax
	mov %ax, %ss

	call kernel_main

	cli

/* Just in case */
halt:
	hlt
	jmp halt
