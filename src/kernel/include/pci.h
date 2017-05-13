/*
 *
 * Elarix
 * src/kernel/dev/pci/pci.h
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

#ifndef _PCI_H
#define _PCI_H

#include <list.h>

#define PCI_BUSES	256
#define PCI_DEVICES	32
#define PCI_FUNCTIONS	8

#define PCI_ID_ANY	~0

#define PCI_ID(v, sv, d, sd, c, sc, pi) \
	{ \
		.vendor = (v), \
		.sub_vendor = (sv), \
		.device = (d), \
		.sub_device = (sd), \
		.class = (c), \
		.sub_class = (sc), \
		.prog_if = (pi) \
	}

struct pci_cfg {
	u16	vendor;
	u16	device;
	u16	cmd;
	u16	status;
	u8	revision;
	u8	prog_if;
	u8	sub_class;
	u8	class;
	u8	cache_line_size;
	u8	latency_timer;
	u8	header_type;
	u8	bist;
	u32	bar_0;
	u32	bar_1;
	u32	bar_2;
	u32	bar_3;
	u32	bar_4;
	u32	bar_5;
	u32	unused;
	u16	sub_vendor;
	u16	sub_device;
	u32	unused1[3];
	u8	int_line;
	u8	int_pin;
	u8	min_gnt;
	u8	max_lat;
} __attribute__ ((packed));

struct pci_id {
	u16	vendor, sub_vendor;
	u16	device, sub_device;
	u8	class, sub_class;
	u8	prog_if;
};

void pci_outd(char busid[8], u32 reg, u32 val);

#endif
