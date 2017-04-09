/*
 *
 * Elarix
 * src/kernel/arch/x86/boot/entry.S
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

.section .mboot

.set MB_MAGIC,		0x1BADB002
#.set MB_FLAGS,		(1 << 0) | (1 << 1) | (1 << 2)
.set MB_FLAGS,		(1 << 0) | (1 << 1)

.align 4
mboot_header:
	.long MB_MAGIC
	.long MB_FLAGS
	.long -(MB_MAGIC + MB_FLAGS)

.section .bss
.global mboot
.align 4
.lcomm mboot, 8


.section .text

entry:
	/* Disable interrupts */
	cli

	/* Check the Multiboot magic number (FIXME) */
	/* cmpl $0x1BADB002, (%eax)
	jne nomboot */

	/* Save the Multiboot info struct pointer */
	mov %ebx, mboot

#ifdef CONFIG_X86_PAE
	/* Check if CPUID is available */
	pushfl
	pop %eax
	bt $21, %eax
	jb nocpuid

	/* Check if PAE is available */
	mov $0x01, %eax
	cpuid
	bt $6, %edx
	jnc nopae
#endif

#ifdef ARCH_x86_64
	/* Check if extended CPUID is available */
	mov $0x80000000, %eax
	cpuid
	cmp $0x80000001, %eax
	jb nolm

	/* Check if long mode is available */
	mov $0x80000001, %eax
	cpuid
	bt $29, %edx
	jnc nolm
#endif

	/* Disable paging (required by Intel manual) */
	mov %cr0, %eax
	btr $31, %eax /* CR0.PG */
	mov %eax, %cr0

	/* TODO Enable SSE and SSE2 if available */

	/* Enable PAE and SSE */
	mov %cr4, %eax
#ifdef CONFIG_X86_PAE
	bts $5, %eax /* CR4.PAE */
#endif
	bts $7, %eax /* CR4.PGE */ /* TEMP */
	#bts $9, %eax /* CR4.OSFXSR */
	#bts $10, %eax /* CR4.OSXMMEXCPT */
	mov %eax, %cr4

	/* Load paging base address */
	call paging_init
	mov $0x200000, %eax
	mov %eax, %cr3

#ifdef ARCH_x86_64
	/* Enable long mode */
	mov $0xC0000080, %ecx
	rdmsr
	bts $8, %eax /* IA32_EFER.LME */
	wrmsr
#endif

	/* Enable paging */
	mov %cr0, %eax
	bts $31, %eax /* CR0.PG */
	bts $16, %eax /* CR0.WP */
	mov %eax, %cr0

	call gdt_init

#ifdef ARCH_i386
	jmp kernel_start
#endif
#ifdef ARCH_x86_64
	ljmp $0x08, $kernel_start
#endif

nomboot:
	mov $msg_nomboot, %esi
	jmp early_panic

nocpuid:
	mov $msg_nocpuid, %esi
	jmp early_panic

nopae:
	mov $msg_nopae, %esi
	jmp early_panic

nolm:
	mov $msg_nolm, %esi
	jmp early_panic

nosse:
	mov $msg_nosse, %esi
	jmp early_panic

nosse2:
	mov $msg_nosse2, %esi
	jmp early_panic

early_panic:
	/* Hide cursor */
	mov $0x0A, %al
	mov $0x3D4, %dx
	outb %al, %dx
	mov $0x1D, %al
	mov $0x3D5, %dx
	outb %al, %dx

	mov $0xB8000, %ecx
	mov $0x07, %ah

clear_loop:
	mov $0, %al
	cmp $2000, %eax
	je print_start
	mov %ax, (%ecx)

	add $0x02, %ecx
	inc %eax
	jmp clear_loop

print_start:
	mov $0xB8000, %ecx
	mov $0x07, %ah

print_loop:
	mov (%esi), %al
	cmp $0, %al
	je halt
	mov %ax, (%ecx)

	add $0x02, %ecx
	inc %esi
	jmp print_loop

halt:
	hlt
	jmp halt


.section .rodata

msg_nomboot:
	.ascii "Invalid Multiboot magic number\0"
msg_nocpuid:
	.ascii "CPUID is not supported by your processor!\0"
msg_nopae:
	.ascii "PAE is not supported by your processor!\0"
msg_nolm:
	.ascii "Long mode is not supported by your processor!\0"
msg_nosse:
	.ascii "SSE is not supported by your processor!\0"
msg_nosse2:
	.ascii "SSE2 is not supported by your processor!\0"
msg_panic:
	.ascii "Unable to continue. The system has been halted\0"
