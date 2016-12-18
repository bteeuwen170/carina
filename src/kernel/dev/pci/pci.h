/*
 *
 * Elara
 * src/kernel/dev/pci/pci.h
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

#ifndef _PCI_H
#define _PCI_H

#include <list.h>

#define PCI_BUSES	256
#define PCI_DEVICES	32
#define PCI_FUNCTIONS	8

/* PCI Config space structure */
struct pci_config_space {
	u16	vendor;
	u16	device;
	u16	cmd;
	u16	status;
	u8	revision;
	u8	prog_if;
	u8	sub_class;
	u8	base_class;
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

struct pci_dev {
	struct list_head l;

	u16			bus, dev, func;

	struct pci_driver	*driver;
	struct pci_config_space	*cfg;
};

#define PCI_ANY_ID	~(0)

#define PCI_DEVICE(v, d, sv, sd, bc, sc, pi) \
	.vendor = (v), \
	.device = (d), \
	.sub_vendor = (sv), \
	.sub_device = (sd), \
	.base_class = (bc), \
	.sub_class = (sc), \
	.prog_if = (pi)

struct pci_dev_id {
	u16	vendor, device;
	u16	sub_vendor, sub_device;

	u8	base_class, sub_class, prog_if;
};

struct pci_driver {
	struct list_head l;

	const char		*name;
	const struct pci_dev_id	*ids;

	int	(*probe) (struct pci_dev *);
	void	(*fini) (struct pci_dev *);
};

u32 pci_ind(struct pci_dev *dev, u32 reg);
void pci_outd(struct pci_dev *dev, u32 reg, u32 val);

void pci_driver_reg(struct pci_driver *driver);
void pci_driver_unreg(struct pci_driver *driver);

void pci_init(void);
void pci_exit(void);

#endif
