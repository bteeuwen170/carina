/*
 *
 * Carina
 * ISR Handler
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

.code64
.section .text

.extern _int_handler

.macro ISR index
	.global _isr\index
	_isr\index:
		cli
		push $0x00
		push $\index
		jmp int_handler
.endm

.macro ISR_ERR index
	.global _isr\index
	_isr\index:
		cli
		push $\index
		jmp int_handler
.endm

.macro IRQ ident index
	.global _irq\ident
	_irq\ident:
		cli
		push $0x00
		push $\index
		jmp int_handler
.endm

ISR			 0
ISR			 1
ISR			 2
ISR			 3
ISR			 4
ISR			 5
ISR			 6
ISR			 7
ISR_ERR		 8
ISR			 9
ISR_ERR		10
ISR_ERR		11
ISR_ERR		12
ISR_ERR		13
ISR_ERR		14
ISR			15
ISR			16
ISR			17
ISR			18
ISR			19
ISR			20
ISR			21
ISR			22
ISR			23
ISR			24
ISR			25
ISR			26
ISR			27
ISR			28
ISR			29
ISR			30
ISR			31

IRQ		 0, 32
IRQ		 1, 33
IRQ		 2, 34
IRQ		 3, 35
IRQ		 4, 36
IRQ		 5, 37
IRQ		 6, 38
IRQ		 7, 39
IRQ		 8, 40
IRQ		 9, 41
IRQ		10, 42
IRQ		11, 43
IRQ		12, 44
IRQ		13, 45
IRQ		14, 46
IRQ		15, 47

.align 0x04
int_handler:
	push %rax
	push %rbx
	push %rcx
	push %rdx
	push %rdi
	push %rsi
	push %rbp
	push %r8
	push %r9
	push %r10
	push %r11
	push %r12
	push %r13
	push %r14
	push %r15

	mov %dx, %ax
	push %rax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %rsp, %rdi
	mov %rsp, %rbx
	and $~0xF, %rsp
	xor %rbp, %rbp
	call _int_handler
	mov %rbx, %rsp

	pop %rbx
	mov %bx, %ds
	mov %bx, %es
	mov %bx, %fs
	mov %bx, %gs

	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rbp
	pop %rsi
	pop %rdi
	pop %rdx
	pop %rcx
	pop %rbx
	pop %rax

	add $16, %rsp
	iretq
