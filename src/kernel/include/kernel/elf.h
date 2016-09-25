/*
 *
 * Carina
 * src/kernel/include/kernel/elf.h
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

#ifndef __KERNEL_ELF_H_
#define __KERNEL_ELF_H_

struct elf_header32 {
	u8			magic[16];
	u16			machine;
	u16			version;
	vaddr32_t	entry;
	u32			program_off;
	u32			section_off;
	u32			flags;
	u16			header_size;
	u16			program_size;
	u16			program_len;
	u16			section_size;
	u16			section_len;
	u16			section_names;
} __attribute__ ((packed));

struct elf_header64 {
	u8			magic[16];
	u16			machine;
	u16			version;
	vaddr64_t	entry;
	u64			program_off;
	u64			section_off;
	u32			flags;
	u16			header_size;
	u16			program_size;
	u16			program_len;
	u16			section_size;
	u16			section_len;
	u16			section_names;
} __attribute__ ((packed));

struct elf_program32 {
	u32			type;
	u32			off;
	vaddr32_t	vaddr;
	paddr32_t	paddr;
	u32			size;
	u32			size_mem;
	u32			flags;
	u32			align;
} __attribute__ ((packed));

struct elf_program64 {
	u32			type;
	u32			flags;
	u64			off;
	vaddr64_t	vaddr;
	paddr64_t	paddr;
	u64			size;
	u64			size_mem;
	u64			align;
} __attribute__ ((packed));

struct elf_section32 {
	u32			name;
	u32			type;
	u32			flags;
	vaddr32_t	vaddr;
	u32			off;
	u32			size;
	u64			reserved0;
} __attribute__ ((packed));

struct elf_section64 {
	u32			name;
	u32			type;
	u64			flags;
	vaddr64_t	vaddr;
	u64			off;
	u64			size;
	u64			reserved[3];
} __attribute__ ((packed));

#endif
