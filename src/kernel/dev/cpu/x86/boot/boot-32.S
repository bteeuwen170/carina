/*
 *
 * Elarix
 * src/kernel/dev/cpu/x86/boot/boot-32.S
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
	mov $pdpte, %eax
	;call paging_init
	;mov $0x200000, %eax
	mov %eax, %cr3

	/* Enable paging */
	mov %cr0, %eax
	bts $16, %eax /* CR0.WP */
	mov %eax, %cr0

	/* XXX Do we have to load the segment registers as well? */

	/* TODO Use gdt_init */
	lgdt gdt

	jmp start


.section .data

.align 4096
gdt_sd:
/* 0x00 - Reserved */
	.quad 0x0000000000000000
/* 0x08 - Code SV */
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0b10011000	/* P,  DPL, 1, 1, C, -, - */
	.byte 0b00000000	/* -, D, L, -, -, -, -, - */
	.byte 0x00
/* 0x10 - Data SV */
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0b10010000	/* P, -, -, 1, 0, -, -, - */
	.byte 0b00000000	/* -, -, -, -, -, -, -, - */
	.byte 0x00
/* 0x18 - Code USR */
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0b11111000	/* P,  DPL, 1, 1, C, -, - */
	.byte 0b00000000	/* -, D, L, -, -, -, -, - */
	.byte 0x00
/* 0x20 - Data USR */
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0b10010010	/* P, -, -, 1, 0, -, -, - */
	.byte 0b00000000	/* -, -, -, -, -, -, -, - */
	.byte 0x00
gdt:
	.word . - gdt_sd - 1	/* Limit */
	.long gdt_sd		/* Base address */

.align 4096
/* Controls 1 GB */
pdpte:
	.long (pde + 3)
	.fill 511, 8, 0
/* Controls 2 MB */
pde:
	.long (pte + 3)
	.fill 511, 8, 0
/* Controls 4 KB */
pte:
	i = 0x00
	.rept 512
		.long (i + 3)
		i = i + 4096
	.endr


.section .text

start:
	call kernel_main

	cli

/* Just in case */
halt:
	hlt
	jmp halt
