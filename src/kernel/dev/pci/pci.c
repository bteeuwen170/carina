/*
 *
 * Elarix
 * src/kernel/dev/pci/pci.c
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

#include <dev.h>
#include <errno.h>
#include <kernel.h>
#include <list.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <stdlib.h>

static const char devname[] = "pci";

#if 1
struct {
	u8		class;
	u8		sub_class;
	u8		prog_if;
	const char	*name;
} device_names[] = {
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
#endif

#if 0
static void pci_dev_reg(struct pci_dev *card)
{
	u8 i;


	list_init(&card->l);
	list_add(&pci_devices, &card->l);
}
#endif

static u32 pci_ind(u16 bus, u16 dev, u16 func, u32 reg)
{
	io_outd(0xCF8, 0x80000000L | ((u32) bus << 16) | ((u32) dev << 11) |
			((u32) func << 8) | (reg & ~3));

	return io_ind(0xCFC + (reg & 3));
}

static void pci_outd(u16 bus, u16 dev, u16 func, u32 reg, u32 val)
{
	io_outd(0xCF8, 0x80000000L | ((u32) bus << 16) | ((u32) dev << 11) |
			((u32) func << 8) | (reg & ~3));

	io_outd(0xCFC, val);
}

static int pci_config(u16 bus, u16 dev, u16 func)
{
	struct driver *drip;
	struct device *devp;
	struct pci_cfg *pcp;
	struct pci_id *pdip;
	size_t i;
	int res = 0;

	if ((pci_ind(bus, dev, func, 0) & 0xFFFF) == 0xFFFF)
		return 0;

	if (!(pcp = kmalloc(sizeof(struct pci_cfg)))) {
		res = -ENOMEM;
		goto err;
	}

	for (i = 0; i < 4; i++) {
		*(uintptr_t *) ((uintptr_t) pcp + i * 16) =
			pci_ind(bus, dev, func, i * 16);
		*(uintptr_t *) ((uintptr_t) pcp + i * 16 + 4) =
			pci_ind(bus, dev, func, i * 16 + 4);
		*(uintptr_t *) ((uintptr_t) pcp + i * 16 + 8) =
			pci_ind(bus, dev, func, i * 16 + 8);
		*(uintptr_t *) ((uintptr_t) pcp + i * 16 + 12) =
			pci_ind(bus, dev, func, i * 16 + 12);
	}

	list_for_each(drip, &drivers, l) {
		if (drip->busid != BUS_PCI)
			continue;

		/* TODO Find out how to check other values as well */
		for (pdip = (struct pci_id *) drip->bus;
				pdip->device || pdip->vendor; pdip++)
			if (pdip->vendor == pcp->vendor &&
					pdip->device == pcp->device &&
					pdip->class == pcp->class)
				goto found;
	}

	goto err;

found:
	/* XXX Does controller apply to all pci devices? */
	if ((res = device_reg(drip, &devp, D_CONTROLLER)) < 0)
		goto err;

	for (i = 0; i < (sizeof(device_names) / sizeof(device_names[0])); i++) {
		if (device_names[i].class == pcp->class &&
				device_names[i].sub_class == pcp->sub_class) {
			devp->name = device_names[i].name;

			break;
		}
	}

	devp->bus = pcp;

	return 0;

err:
	kfree(pcp);

	return res;
}

int pci_init(void)
{
	u16 bus, dev, func;
	int res;

	for (bus = 0; bus < PCI_BUSES; bus++)
		for (dev = 0; dev < PCI_DEVICES; dev++)
			for (func = 0; func < PCI_FUNCTIONS; func++)
				if ((res = pci_config(dev, bus, func)) < 0)
					return res;

	return 0;
}

void pci_exit(void)
{
	/* TODO Free all structures */
}

MODULE(pci, &pci_init, &pci_exit);
