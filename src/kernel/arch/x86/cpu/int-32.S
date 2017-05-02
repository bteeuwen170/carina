/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/int-32.S
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

.code32

.section .text

.extern _isr

.altmacro

.macro INT i
	.global  _int\i
	_int\i:
		cli
		push 0
		push $\i
		jmp isr
.endm

.macro INT_ERR i
	.global  _int\i
	_int\i:
		cli
		push $\i
		jmp isr
.endm

/* Exceptions */
INT	0
INT	1
INT	2
INT	3
INT	4
INT	5
INT	6
INT	7
INT_ERR	8
INT	9
INT_ERR	10
INT_ERR	11
INT_ERR	12
INT_ERR	13
INT_ERR	14
INT	15
INT	16
INT_ERR	17
INT	18
INT	19
INT	20
INT	21
INT	22
INT	23
INT	24
INT	25
INT	26
INT	27
INT	28
INT	29
INT	30
INT	31

/* IRQs */
i = 32
.rept 224
	INT %i
	i = i + 1
.endr

isr:
	pusha

	xor %eax, %eax
	mov %ds, %ax
	push %eax

	mov %esp, %edi
	call _isr

	#mov %rbx, %rsp
	pop %eax

	popa

	add $0x10, %esp
	iret
