/*
 *
 * Elarix
 * src/kernel/arch/x86/boot/boot-64.S
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

.code64

.section .text

.global kernel_start
kernel_start:
	/* Intel Software Developer's Manual V3 6.14.3 */
	xor %ax, %ax
	mov %ax, %ss

	mov mboot, %rdi
	call kernel_main

/* Just in case */
	cli
halt:
	hlt
	jmp halt
