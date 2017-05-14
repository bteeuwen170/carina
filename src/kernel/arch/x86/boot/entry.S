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
	bts $7, %eax /* CR4.PGE */
	#bts $9, %eax /* CR4.OSFXSR */
	#bts $10, %eax /* CR4.OSXMMEXCPT */
	mov %eax, %cr4

	/* Load paging base address */
#if ARCH_x86_64
	mov $(_pml4 - VM_ADDR), %eax
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

/* FIXME Don't hardcode 6/8 MB */
.align 4096
#ifdef ARCH_x86_64
.global _pml4
_pml4:
	.quad pdpt_lo - VM_ADDR + 0b000011
	.fill 510, 8, 0
	.quad pdpt_hi - VM_ADDR + 0b000011
pdpt_lo:
	.quad pdt - VM_ADDR + 0b00000011
	.fill 511, 8, 0
pdpt_hi:
	.fill 510, 8, 0
	.quad pdt - VM_ADDR + 0b00000011
	.quad 0
pdt:
	.quad pt_0 - VM_ADDR + 0b00000011
	.quad pt_1 - VM_ADDR + 0b00000011
	.quad pt_2 - VM_ADDR + 0b00000011
	.quad pt_3 - VM_ADDR + 0b00000011
	.quad pt_4 - VM_ADDR + 0b00000011
	.quad pt_5 - VM_ADDR + 0b00000011
	.quad pt_6 - VM_ADDR + 0b00000011
	.quad pt_7 - VM_ADDR + 0b00000011
	.quad pt_8 - VM_ADDR + 0b00000011
	.quad pt_9 - VM_ADDR + 0b00000011
	.quad pt_10 - VM_ADDR + 0b00000011
	.quad pt_11 - VM_ADDR + 0b00000011
	.quad pt_12 - VM_ADDR + 0b00000011
	.quad pt_13 - VM_ADDR + 0b00000011
	.quad pt_14 - VM_ADDR + 0b00000011
	.quad pt_15 - VM_ADDR + 0b00000011
	.quad pt_16 - VM_ADDR + 0b00000011
	.quad pt_17 - VM_ADDR + 0b00000011
	.quad pt_18 - VM_ADDR + 0b00000011
	.quad pt_19 - VM_ADDR + 0b00000011
	.quad pt_20 - VM_ADDR + 0b00000011
	.quad pt_21 - VM_ADDR + 0b00000011
	.quad pt_22 - VM_ADDR + 0b00000011
	.quad pt_23 - VM_ADDR + 0b00000011
	.quad pt_24 - VM_ADDR + 0b00000011
	.quad pt_25 - VM_ADDR + 0b00000011
	.quad pt_26 - VM_ADDR + 0b00000011
	.quad pt_27 - VM_ADDR + 0b00000011
	.quad pt_28 - VM_ADDR + 0b00000011
	.quad pt_29 - VM_ADDR + 0b00000011
	.quad pt_30 - VM_ADDR + 0b00000011
	.quad pt_31 - VM_ADDR + 0b00000011
	.quad pt_32 - VM_ADDR + 0b00000011
	.quad pt_33 - VM_ADDR + 0b00000011
	.quad pt_34 - VM_ADDR + 0b00000011
	.quad pt_35 - VM_ADDR + 0b00000011
	.quad pt_36 - VM_ADDR + 0b00000011
	.quad pt_37 - VM_ADDR + 0b00000011
	.quad pt_38 - VM_ADDR + 0b00000011
	.quad pt_39 - VM_ADDR + 0b00000011
	.quad pt_40 - VM_ADDR + 0b00000011
	.quad pt_41 - VM_ADDR + 0b00000011
	.quad pt_42 - VM_ADDR + 0b00000011
	.quad pt_43 - VM_ADDR + 0b00000011
	.quad pt_44 - VM_ADDR + 0b00000011
	.quad pt_45 - VM_ADDR + 0b00000011
	.quad pt_46 - VM_ADDR + 0b00000011
	.quad pt_47 - VM_ADDR + 0b00000011
	.quad pt_48 - VM_ADDR + 0b00000011
	.quad pt_49 - VM_ADDR + 0b00000011
	.quad pt_50 - VM_ADDR + 0b00000011
	.quad pt_51 - VM_ADDR + 0b00000011
	.quad pt_52 - VM_ADDR + 0b00000011
	.quad pt_53 - VM_ADDR + 0b00000011
	.quad pt_54 - VM_ADDR + 0b00000011
	.quad pt_55 - VM_ADDR + 0b00000011
	.quad pt_56 - VM_ADDR + 0b00000011
	.quad pt_57 - VM_ADDR + 0b00000011
	.quad pt_58 - VM_ADDR + 0b00000011
	.quad pt_59 - VM_ADDR + 0b00000011
	.fill 452, 8, 0

.macro PT i
	pt_\i:
		j = \i * 512
		.rept 512
			.quad j * 4096 + 0b100000011
			j = j + 1
		.endr
.endm

PT 0
PT 1
PT 2
PT 3
PT 4
PT 5
PT 6
PT 7
PT 8
PT 9
PT 10
PT 11
PT 12
PT 13
PT 14
PT 15
PT 16
PT 17
PT 18
PT 19
PT 20
PT 21
PT 22
PT 23
PT 24
PT 25
PT 26
PT 27
PT 28
PT 29
PT 30
PT 31
PT 32
PT 33
PT 34
PT 35
PT 36
PT 37
PT 38
PT 39
PT 40
PT 41
PT 42
PT 43
PT 44
PT 45
PT 46
PT 47
PT 48
PT 49
PT 50
PT 51
PT 52
PT 53
PT 54
PT 55
PT 56
PT 57
PT 58
PT 59
#elif defined(CONFIG_X86_PAE)
/* TODO */
#else
pdt:
	.long pt_0 - VM_ADDR + 0b00000011
	.long pt_1 - VM_ADDR + 0b00000011
	.fill 766, 4, 0
	.long pt_0 - VM_ADDR + 0b00000011
	.long pt_1 - VM_ADDR + 0b00000011
	.fill 254, 4, 0
pt_0:
	i = 0
	.rept 1024
		.long i * 4096 + 0b100000011
		i = i + 1
	.endr
pt_1:
	i = 1024
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
