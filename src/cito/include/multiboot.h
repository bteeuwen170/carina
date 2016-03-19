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
	u32 flags;
	u32 mem_lower, mem_higher;
	u32 root;
	u32 cmd;
	u32 mods_cnt, mods_addr;
	u32 sym[4];
	u32 mmap_len, mmap_addr;
	u32 drives_len, drives_addr;
	u32 cfg_table;
	u32 boot_loader;
	u32 apm_table;
	u32 vbe_ctrl_info, vbe_mode_info, vbe_info;
	u32 vbe_iface_seg, vbe_iface_base, vbe_iface_len;
} mbis_t;

#endif
