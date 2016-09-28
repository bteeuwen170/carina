/*
 *
 * Carina
 * src/kernel/dev/video/vesa.h
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

#ifndef _VESA_H
#define _VESA_H

struct vbe_info {
//	char[4]	signature = "VESA";
//	u16	version = 0x0300;
	u32	unused;
	u32	capabilities;
	u32	video_modes;
	u16	video_mem;
	char	unused1[492];
} __attribute__ ((packed));

struct vbe_info_mode {
	u16	attr;
	u32	unused;
	u16	win_size;
	u16	wina_segment;
	u16	winb_segment;
	u32	unused1;
	u16	bip_scan_line;

	u16	x_res;
	u16	y_res;
	u16	unused2;
	u8	mem_planes;
	u8	byp_pixel;
	u8	unused3;
	u8	mem_model;
	u8	unused4;
	u8	images;
	u8	unused5;

	u8	red_mask;
	u8	red_pos;
	u8	green_mask;
	u8	green_pos;
	u8	blue_mask;
	u8	blue_pos;
	u16	unused6;
	u8	dc_attr;

	u32	fb;
	u16	unused7[3];

	u16	byp_scan_line;
	u8	bank_images;
	u8	linear_images;
	u8	linear_red_mask;
	u8	linear_red_pos;
	u8	linear_green_mask;
	u8	linear_green_pos;
	u8	linear_blue_mask;
	u8	linear_blue_pos;
	u16	unused8;
	u32	pclock_max;

	u8	reserved[189];
} __attribute__ ((packed));

#endif
