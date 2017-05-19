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

#ifdef ARCH_x86_64
.set VM_ADDR,		0xFFFFFFFF80000000
#else
.set VM_ADDR,		0xC0000000
#endif

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

.align 4
.global mboot
.lcomm mboot, 8


.section .text

entry:
	/* Disable interrupts */
	cli

	/* Check the Multiboot magic number (FIXME) */
	/* cmpl $0x1BADB002, (%eax)
	jne nomboot */

	/* Save the Multiboot info struct pointer */
	mov %ebx, (mboot - VM_ADDR)

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
	#bts $7, %eax /* CR4.PGE */ /* XXX Maybe? */
	#bts $9, %eax /* CR4.OSFXSR */
	#bts $10, %eax /* CR4.OSXMMEXCPT */
	mov %eax, %cr4

	/* Load paging base address */
#if ARCH_x86_64
	mov $(pml4 - VM_ADDR), %eax
#elif defined(CONFIG_X86_PAE)
	mov $(pdpt - VM_ADDR), %eax
#else
	mov $(pdt - VM_ADDR), %eax
#endif
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

	lgdt (gdtp - VM_ADDR)

#ifdef ARCH_i386
	jmp _cpu_init
#endif
#ifdef ARCH_x86_64
	ljmp $0x08, $(_cpu_init - VM_ADDR)
#endif

nomboot:
	mov $(msg_nomboot - VM_ADDR), %esi
	jmp early_panic

nocpuid:
	mov $(msg_nocpuid - VM_ADDR), %esi
	jmp early_panic

nopae:
	mov $(msg_nopae - VM_ADDR), %esi
	jmp early_panic

nolm:
	mov $(msg_nolm - VM_ADDR), %esi
	jmp early_panic

nosse:
	mov $(msg_nosse - VM_ADDR), %esi
	jmp early_panic

nosse2:
	mov $(msg_nosse2 - VM_ADDR), %esi
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


.section .data

#ifdef ARCH_x86_64
gdt:
	.quad 0				/* 0x00 */
	.quad 0x002098000000FFFF	/* 0x08 - Code SV */
	.quad 0x000090000000FFFF	/* 0x10 - Data SV */
	.quad 0x0020F8000000FFFF	/* 0x18 - Code USR */
	.quad 0x0000F2000000FFFF	/* 0x20 - Data USR */
	/* .quad 0x00FFFF000000FFFF */	/* 0x28 - TSS */
gdtp:
	.word . - gdt - 1
	.quad gdt - VM_ADDR
#else
gdt:
	.quad 0				/* 0x00 */
	.long 0x0000FFFF		/* 0x08 - Code SV */
	.long 0x00009800
	.long 0x0000FFFF		/* 0x10 - Data SV */
	.long 0x00009000
	.long 0x0000FFFF		/* 0x18 - Code USR */
	.long 0x0000F800
	.long 0x0000FFFF		/* 0x20 - Data USR */
	.long 0x0000F200
	/* .long 0x00FFFF0000FF */	/* 0x28 - TSS */

gdtp:
	.word . - gdt - 1
	.long gdt - VM_ADDR
#endif

/* FIXME Don't do this statically */
.align 4096
#ifdef ARCH_x86_64
pml4:
	.quad pdtp_lo - VM_ADDR + 0b000011
	.fill 509, 8, 0
	.quad pml4 - VM_ADDR + 0b000011
	.quad pdtp_hi - VM_ADDR + 0b000011
pdtp_lo:
	.quad pdt_lo - VM_ADDR + 0b00000011
	.fill 511, 8, 0
pdtp_hi:
	.fill 510, 8, 0
	.quad pdt_hi - VM_ADDR + 0b00000011
	.quad 0
pdt_lo:
	.quad pt_0 - VM_ADDR + 0b00000011
	.fill 511, 8, 0
pdt_hi:
	.quad pt_0 - VM_ADDR + 0b00000011
	.fill 511, 8, 0
pt_0:
	j = 0
	.rept 511
		.quad j * 4096 + 0b100000011
		j = j + 1
	.endr
	.quad 0
#elif defined(CONFIG_X86_PAE)
/* TODO */
#else
pdt:
	.long pt_0 - VM_ADDR + 0b00000011
	.fill 767, 4, 0
	.long pt_0 - VM_ADDR + 0b00000011
	.fill 255, 4, 0
pt_0:
	i = 0
	.rept 1024
		.long i * 4096 + 0b100000011
		i = i + 1
	.endr
#endif


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
