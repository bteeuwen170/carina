/*
 *
 * Carina
 * src/kernel/arch/x86/cpu/task-64.S
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

/* XXX This is temporary */

.code64

.section .text

.global usrmode_enter

usrmode_enter:
	cli
	mov $0x23, %ax
	mov %ax, %dx
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov %rsp, %rax
	push $0x23		/* SS */
	push %rax		/* RSP */
	pushf			/* RFLAGS */
	push $0x1B		/* CS */
	push usermodes_main
	iretq

usermodes_main:
	hlt
	jmp usermodes_main
