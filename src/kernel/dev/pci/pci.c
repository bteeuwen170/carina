/*
 *
 * Elarix
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

#include <kernel.h>
#include <list.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <stdio.h>
#include <stdlib.h>

static const char devname[] = "pci";

static LIST_HEAD(pci_devices);
static LIST_HEAD(pci_drivers);

struct {
	u8		base_class;
	u8		sub_class;
	u8		prog_if;
	const char	*name;
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

u32 pci_ind(struct pci_dev *card, u32 reg)
{
	io_outd(0xCF8, 0x80000000L | ((u32) card->bus << 16) |
			((u32) card->dev << 11) | ((u32) card->func << 8) |
			(reg & ~3));

	return io_ind(0xCFC + (reg & 3));
}

void pci_outd(struct pci_dev *card, u32 reg, u32 val)
{
	io_outd(0xCF8, 0x80000000L | ((u32) card->bus << 16) |
			((u32) card->dev << 11) | ((u32) card->func << 8) |
			(reg & ~3));

	io_outd(0xCFC, val);
}

static void pci_dev_reg(struct pci_dev *card)
{
	u8 i;

	for (i = 0; i < (sizeof(pci_device_table) /
			sizeof(pci_device_table[0])); i++) {
		if ((pci_device_table[i].base_class == card->cfg->base_class &&
				pci_device_table[i].sub_class ==
				card->cfg->sub_class)) {
			dprintf(devname, "detected %#x %#x %s\n",
					card->cfg->vendor, card->cfg->device,
					pci_device_table[i].name);

			break;
		}
	}

	list_init(&card->l);
	list_add(&pci_devices, &card->l);
}

/* static void pci_dev_unreg(struct pci_dev *card)
{
} */

int pci_driver_reg(struct pci_driver *driver)
{
	/* TODO Check if not already present */

	list_init(&driver->l);
	list_add(&pci_drivers, &driver->l);

	return 0;
}

void pci_driver_unreg(struct pci_driver *driver)
{
	/* TODO Check if present */

	list_rm(&driver->l);
}

static struct pci_dev *pci_config(u16 bus, u16 dev, u16 func)
{
	struct pci_dev *card;
	struct pci_driver *driver;
	const struct pci_dev_id *id;
	u8 i;

#if 1
	struct pci_dev tmp;

	tmp.bus = bus;
	tmp.dev = dev;
	tmp.func = func;

	if ((pci_ind(&tmp, 0) & 0xFFFF) == 0xFFFF)
		return NULL;
#else
	if ((pci_ind(card, 0) & 0xFFFF) == 0xFFFF) {
		kfree(card);
		return NULL;
	}
#endif

	card = kmalloc(sizeof(struct pci_dev));

	if (!card)
		goto err;

	card->bus = bus;
	card->dev = dev;
	card->func = func;

	card->cfg = kmalloc(sizeof(struct pci_config_space));

	if (!card->cfg)
		goto err;

	for (i = 0; i < 4; i++) {
		*(uintptr_t *) ((uintptr_t) card->cfg + i * 16) =
			pci_ind(card, i * 16);
		*(uintptr_t *) ((uintptr_t) card->cfg + i * 16 + 4) =
			pci_ind(card, i * 16 + 4);
		*(uintptr_t *) ((uintptr_t) card->cfg + i * 16 + 8) =
			pci_ind(card, i * 16 + 8);
		*(uintptr_t *) ((uintptr_t) card->cfg + i * 16 + 12) =
			pci_ind(card, i * 16 + 12);
	}

	list_for_each(driver, &pci_drivers, l) {
		id = driver->ids;

		/*
		 * TODO
		 * More checks
		 * Seperate function
		 */
		while (id->vendor || id->sub_vendor ||
				id->base_class) {
			if (id->vendor == card->cfg->vendor &&
					id->device == card->cfg->device) {
				card->driver = driver;
				break;
			}

			id++;
		}
	}

	return card;

	/* TODO Provide error information */
err:
	dprintf(devname, KP_ERR "out of memory\n");

	return NULL;
}

int pci_init(void)
{
	struct pci_dev *card;
	u16 bus, dev, func;

	for (bus = 0; bus < PCI_BUSES; bus++) {
		for (dev = 0; dev < PCI_DEVICES; dev++) {
			for (func = 0; func < PCI_FUNCTIONS; func++) {
				card = pci_config(dev, bus, func);

				if (!card)
					continue;

				pci_dev_reg(card);
			}
		}
	}

	list_for_each(card, &pci_devices, l)
		if (card->driver && card->driver->probe)
			card->driver->probe(card);

	return 0;
}

void pci_exit(void)
{
	/* TODO Free all structures */
}

MODULE(pci, &pci_init, &pci_exit);
