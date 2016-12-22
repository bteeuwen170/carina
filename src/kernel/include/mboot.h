/*
 *
 * Elarix
 * src/kernel/include/mboot.h
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

#ifndef _MBOOT_H
#define _MBOOT_H

struct mboot_mmap {
	u32	size;
	u32	addr_lo;
	u32	addr_hi;
	u32	len_lo;
	u32	len_hi;
	u32	type;
} __attribute__ ((packed));

struct mboot_info {
	u32	flags;

	u32	mem_lo;
	u32	mem_hi;

	u32	boot_device;

	u32	cmdline;

	u32	mods_cnt;
	u32	mods_addr;

	u32	elf_num;
	u32	elf_size;
	u32	elf_addr;
	u32	elf_shndx;

	u32	mmap_len;
	u32	mmap_addr;

	u32	drives_len;
	u32	drives_addr;

	u32	config_table;

	u32	boot_loader_name;

	u32	apm_table;

	u32	vbe_control_info;
	u32	vbe_mode_info;
	u16	vbe_mode;
	u16	vbe_interface_seg;
	u16	vbe_interface_off;
	u16	vbe_interface_len;

	/* GRUB should really update the specification */
	u64	framebuffer_addr;
	u32	framebuffer_pitch;
	u32	framebuffer_width;
	u32	framebuffer_height;
	u8	framebuffer_bpp;
	u8	framebuffer_type;

	u32	framebuffer_palette_addr;
	u16	framebuffer_palette_num_colors;

	u8	framebuffer_red_field_position;
	u8	framebuffer_red_mask_size;
	u8	framebuffer_green_field_position;
	u8	framebuffer_green_mask_size;
	u8	framebuffer_blue_field_position;
	u8	framebuffer_blue_mask_size;
} __attribute__ ((packed));

#endif
