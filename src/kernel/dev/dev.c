/*
 *
 * Elara
 * src/kernel/dev/dev.c
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

static const char devname[] = "dev";

struct head *devices = 0;
struct handler *handlers = 0;

static void dev_add(struct dev *dp)
{
	list_adda(

	dp->next = devices;

	devices = dp;
}

struct dev *pci_get(u16 bus, u16 dev, u16 func)
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
	 * It's just so sexy!
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
