/*
 *
 * Elarix
 * src/kernel/include/elf.h
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

#ifndef _ELF_H
#define _ELF_H

#define ELF_H_TYPE_REL	0x01
#define ELF_H_TYPE_EXEC	0x02
#define ELF_H_TYPE_SHR	0x03
#define ELF_H_TYPE_CORE	0x04

#define ELF_P_TYPE_LOAD	0x01

struct elf_header {
	char	magic[4];
	u8	class;
	u8	data;
	u8	version0;
	u8	abi;
	u8	unused0[10];
	u16	machine;
	u32	version1;
} __attribute__ ((packed));

struct elf_header32 {
	struct elf_header head;

	u32	entry;
	u32	program_off;
	u8	unused0[4];
	u32	flags;
	u16	header_size;
	u16	program_size;
	u16	program_len;
	u8	unused1[6];
} __attribute__ ((packed));

struct elf_header64 {
	struct elf_header head;

	u64	entry;
	u64	program_off;
	u8	unused0[8];
	u32	flags;
	u16	header_size;
	u16	program_size;
	u16	program_len;
	u8	unused1[6];
} __attribute__ ((packed));

struct elf_program32 {
	u32	type;
	u32	off;
	u32	vaddr;
	u32	paddr;
	u32	size;
	u32	size_mem;
	u32	flags;
	u32	align;
} __attribute__ ((packed));

struct elf_program64 {
	u32	type;
	u32	flags;
	u64	off;
	u64	vaddr;
	u64	paddr;
	u64	size;
	u64	size_mem;
	u64	align;
} __attribute__ ((packed));

struct elf_section32 {
	u32	name;
	u32	type;
	u32	flags;
	u32	vaddr;
	u32	off;
	u32	size;
	u8	unused0[8];
	u32	align;
	u8	unused1[4];
} __attribute__ ((packed));

struct elf_section64 {
	u32	name;
	u32	type;
	u64	flags;
	u64	vaddr;
	u64	off;
	u64	size;
	u8	unused0[8];
	u64	align;
	u8	unused1[8];
} __attribute__ ((packed));

#endif
