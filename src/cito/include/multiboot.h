/*
 *
 * Carina
 * Header for Multiboot
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

#ifndef __MB_H__
#define __MB_H__

#include <stdint.h>

typedef struct {
	uint32_t flags;
	uint32_t mem_lower, mem_higher;
	uint32_t root;
	uint32_t cmd;
	uint32_t mods_cnt, mods_addr;
	uint32_t sym[4];
	uint32_t mmap_len, mmap_addr;
	uint32_t drives_len, drives_addr;
	uint32_t cfg_table;
	uint32_t boot_loader;
	uint32_t apm_table;
	uint32_t vbe_ctrl_info, vbe_mode_info, vbe_info;
	uint32_t vbe_iface_seg, vbe_iface_base, vbe_iface_len;
} mbis_t;

#endif
