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

#include <stdlib.h>

typedef struct {
	uint32 flags;
	uint32 mem_lower, mem_higher;
	uint32 root;
	uint32 cmd;
	uint32 mods_cnt, mods_addr;
	uint32 sym[4];
	uint32 mmap_len, mmap_addr;
	uint32 drives_len, drives_addr;
	uint32 cfg_table;
	uint32 boot_loader;
	uint32 apm_table;
	uint32 vbe_ctrl_info, vbe_mode_info, vbe_info;
	uint32 vbe_iface_seg, vbe_iface_base, vbe_iface_len;
} mbis_t;

#endif
