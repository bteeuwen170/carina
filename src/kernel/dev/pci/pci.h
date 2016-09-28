/*
 *
 * Carina
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
	u32	base_adr_0;
	u32	base_adr_1;
	u32	base_adr_2;
	u32	base_adr_3;
	u32	base_adr_4;
	u32	base_adr_5;
	u32	unused;
	u16	subsys_vendor;
	u16	subsys;
	u32	unused1[3];
	u8	int_line;
	u8	int_pin;
	u8	min_gnt;
	u8	max_lat;
} __attribute__ ((packed));

/* PCI device structure */
struct pci_dev {
	u16			bus, dev, func;
	struct pci_config_space	*cfg;
	
	/* Device specific structure */
	void			*device;

	struct pci_dev		*next;
};

/* PCI handler structure */
struct pci_handler {
	int			calls, max_calls;

	u16			vendor, device;
	int			(*handler) (struct pci_dev *);
	
	struct pci_handler	*next;
};

u32 pci_ini(u16 bus, u16 dev, u16 func, u32 reg);

void pci_outi(u16 bus, u16 dev, u16 func, u32 reg, u32 val);

struct pci_dev *pci_get(u16 bus, u16 dev, u16 func);

int pci_reghandler(const u16 vendor, const u16 device, int max_calls,
		int (*handler) (struct pci_dev *));

int pci_unreghandler(const u16 vendor, const u16 device);

int pci_scan(void);

#endif
