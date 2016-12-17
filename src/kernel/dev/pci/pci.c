/*
 *
 * Elara
 * src/kernel/dev/pci/pci.c
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

#include <asm/cpu.h>

#include <print.h>

#include <stdio.h>
#include <stdlib.h>

#include "pci.h"

static const char devname[] = "pci";

struct pci_dev *pci_devices = 0;
struct pci_handler *pci_handlers = 0;

struct {
	u8	base_class;
	u8	sub_class;
	u8	prog_if;
	char	*device;
} pci_device_table[] = {
	/* Unclassified Controllers */
	{ 0x00, 0x00, 0x00, "Non-VGA compatible device" },
	{ 0x00, 0x01, 0x00, "VGA compatible device" },

	/* Mass Storage Controllers */
	{ 0x01, 0x00, 0x00, "SCSI controller" },
	{ 0x01, 0x01, 0x00, "IDE controller" },
	{ 0x01, 0x05, 0x00, "ATA (with single DMA) controller" },
	{ 0x01, 0x05, 0x01, "ATA (with chained DMA) controller" },
	{ 0x01, 0x06, 0x00, "SATA controller" },
	{ 0x01, 0x80, 0x00, "Mass storage controller" },

	/* Network Controllers */
	{ 0x02, 0x00, 0x00, "Ethernet controller" },
	{ 0x02, 0x80, 0x00, "Network controller" },

	/* Display Controllers */
	{ 0x03, 0x00, 0x00, "PC compatible VGA controller" },
	{ 0x03, 0x00, 0x01, "PC compatible 8514 controller" },
	{ 0x03, 0x80, 0x00, "Display controller" },

	/* Multimedia Controllers */
	{ 0x04, 0x00, 0x00, "Video controller" },
	{ 0x04, 0x01, 0x00, "Audio controller" },
	{ 0x04, 0x03, 0x00, "Audio device" },
	{ 0x04, 0x80, 0x00, "Multimedia controller" },

	/* Memory Controllers */
	{ 0x05, 0x00, 0x00, "RAM controller" },
	{ 0x05, 0x01, 0x00, "Flash controller" },
	{ 0x05, 0x80, 0x00, "Memory controller" },

	/* Bridges */
	{ 0x06, 0x00, 0x00, "Host bridge" },
	{ 0x06, 0x01, 0x00, "ISA bridge" },
	{ 0x06, 0x02, 0x00, "EISA bridge" },
	{ 0x06, 0x04, 0x00, "PCI bridge" },
	{ 0x06, 0x05, 0x00, "PCMCIA bridge" },
	{ 0x06, 0x80, 0x00, "Device bridge" },

	/* Communication Controllers */
	{ 0x07, 0x00, 0x00, "8250 serial controller" },
	{ 0x07, 0x00, 0x01, "16450 serial controller" },
	{ 0x07, 0x00, 0x02, "16550 serial controller" },
	{ 0x07, 0x01, 0x00, "Parallel controller" },
	{ 0x07, 0x01, 0x01, "Bidirectional parallel controller" },
	{ 0x07, 0x01, 0x02, "ECP parallel controller" },
	{ 0x07, 0x80, 0x02, "Communication controller" },

	/* Generic System Peripherals */
	{ 0x08, 0x00, 0x00, "8259 PIC" },
	{ 0x08, 0x00, 0x01, "ISA PIC" },
	{ 0x08, 0x00, 0x02, "EISA PIC" },
	{ 0x08, 0x00, 0x03, "I/O APIC" },
	{ 0x08, 0x01, 0x00, "8259 DMA" },
	{ 0x08, 0x01, 0x01, "ISA DMA" },
	{ 0x08, 0x01, 0x02, "EISA DMA" },
	{ 0x08, 0x02, 0x00, "8259 timer" },
	{ 0x08, 0x02, 0x01, "ISA timer" },
	{ 0x08, 0x02, 0x02, "EISA timer" },
	{ 0x08, 0x02, 0x03, "HPET timer" },
	{ 0x08, 0x03, 0x00, "CMOS RTC" },
	{ 0x08, 0x03, 0x01, "ISA RTC" },
	{ 0x08, 0x80, 0x00, "Generic system peripheral" },

	/* Input Devices */
	{ 0x09, 0x00, 0x00, "Keyboard device" },
	{ 0x09, 0x80, 0x00, "Input device" },

	/* Serial bus controllers */
	{ 0x0C, 0x03, 0x00, "UHCI USB controller" },
	{ 0x0C, 0x03, 0x10, "OHCI USB controller" },
	{ 0x0C, 0x03, 0x20, "EHCI USB controller" },
	{ 0x0C, 0x03, 0x30, "XHCI USB controller" },
	{ 0x0C, 0x03, 0x80, "USB controller" },
	{ 0x0C, 0x03, 0xFE, "USB controller" },
	{ 0x0C, 0x80, 0x00, "Serial bus controller" }
};

#define	PCI_DEVTABLE_SIZE \
		(sizeof(pci_device_table) / sizeof(pci_device_table[0]))

/* TODO Use struct */
u32 pci_ind(u16 bus, u16 dev, u16 func, u32 reg)
{
	io_outd(0xCF8, 0x80000000L | ((u32) bus << 16) | ((u32) dev << 11) |
			((u32) func << 8) | (reg & ~3));

	return io_ind(0xCFC + (reg & 3));
}

/* TODO Use struct */
void pci_outd(u16 bus, u16 dev, u16 func, u32 reg, u32 val)
{
	io_outd(0xCF8, 0x80000000L | ((u32) bus << 16) | ((u32) dev << 11) |
			((u32) func << 8) | (reg & ~3));

	io_outd(0xCFC, val);
}

static void pci_add(struct pci_dev *dp)
{
	dp->next = pci_devices;

	pci_devices = dp;
}

struct pci_dev *pci_get(u16 bus, u16 dev, u16 func)
{
	struct pci_dev *cur = pci_devices;

	while (cur != NULL) {
		if (cur->bus == bus && cur->dev == dev && cur->func == func)
			return cur;

		cur = cur->next;
	}

	return NULL;
}

struct pci_handler *pci_gethandler(const u16 vendor, const u16 device)
{
	struct pci_handler *cur = pci_handlers;

	while (cur != NULL) {
		if (cur->vendor == vendor && cur->device == device) {
			return cur;
		}

		cur = cur->next;
	}

	return NULL;
}

int pci_reghandler(const u16 vendor, const u16 device, int max_calls,
		int (*handler) (struct pci_dev *))
{
	if (pci_gethandler(vendor, device) != NULL)
		return 1;

	struct pci_handler *cur = kmalloc(sizeof(struct pci_handler));

	if (!cur)
		return 1;

	cur->max_calls = max_calls;
	cur->vendor = vendor;
	cur->device = device;
	cur->handler = handler;

	cur->next = pci_handlers;

	pci_handlers = cur;

	return 0;
}

/* TODO Needs some polishing */
int pci_unreghandler(const u16 vendor, const u16 device)
{
	struct pci_handler *prev;
	struct pci_handler *cur = pci_handlers;

	while (cur != NULL) {
		if (cur->vendor == vendor && cur->device == device) {
			prev->next = cur->next;
			kfree(cur);

			return 0;
		}

		prev = cur;
		cur = cur->next;
	}

	return 1;
}

static void pci_config(u16 bus, u16 dev, u16 func)
{
	struct pci_dev *dp;
	struct pci_config_space *cfg;
	struct pci_handler *handler;
	u8 i, j;

	if ((pci_ind(bus, dev, func, 0) & 0xFFFF) == 0xFFFF)
		return;

	cfg = kmalloc(sizeof(struct pci_config_space));

	if (!cfg)
		goto err;

	/*
	 * Copyright 2011 Daniel Bittman
	 * This is from
	 * https://github.com/dbittman/seakernel/blob/master/
	 * 		arch/x86_64/drivers/bus/pci.c
	 *
	 * TODO Replace with own elegant version
	 */
	for (i = 0; i < 64; i += 16) {
		*(u32 *) ((unsigned long) cfg + i) = pci_ind(bus, dev, func, i);
		*(u32 *) ((unsigned long) cfg + i + 4) =
			pci_ind(bus, dev, func, i + 4);
		*(u32 *) ((unsigned long) cfg + i + 8) =
			pci_ind(bus, dev, func, i + 8);
		*(u32 *) ((unsigned long) cfg + i + 12) =
			pci_ind(bus, dev, func, i + 12);
	}

	for (i = 0; i < PCI_DEVTABLE_SIZE; i++) {
		if (pci_device_table[i].base_class != cfg->base_class)
			continue;

		for (j = i; j < PCI_DEVTABLE_SIZE; j++)
			if (pci_device_table[j].sub_class == cfg->sub_class)
				break;

		/* TODO More verbose */
		kprintf(KP_INFO, devname, "detected %#x %#x %s\n", cfg->vendor,
				cfg->device, pci_device_table[j].device);

		break;
	}

	dp = kmalloc(sizeof(struct pci_dev));

	if (!dp)
		goto err;

	dp->bus = bus;
	dp->dev = dev;
	dp->func = func;
	dp->cfg = cfg;

	pci_add(dp);

	handler = pci_gethandler(cfg->vendor, cfg->device);

	if (handler->handler && handler->calls < handler->max_calls) {
		handler->handler(dp);
		handler->calls++;
	}

	return;

	/* TODO Provide error information */
err:
	kprintf(KP_ERR, devname, "Out of memory");
}

/* TODO Error messages */
int pci_scan(void)
{
	u16 bus, dev, func;

	if (pci_devices)
		return 1;

	/* Not like anyone has 256 PCI buses */
	for (bus = 0; bus < PCI_BUSES; bus++) {
		for (dev = 0; dev < PCI_DEVICES; dev++) {
			for (func = 0; func < PCI_FUNCTIONS; func++) {
				pci_config(bus, dev, func);
			}
		}
	}

	return 0;
}
