/*
 *
 * Elarix
 * src/kernel/dev/block/ide.c
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

#include <ata.h>
#include <delay.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <kernel.h>
#include <list.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <stdlib.h>
#include <string.h>

#include "ide.h"

static const char devname[] = "ide";

static struct driver ide_driver;

static void ide_outb(struct ide_device *idevp, u8 reg, u8 data)
{
	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(idevp, ATA_REG_CTRL, idevp->nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		io_outb(idevp->base + reg - 0x00, data);
	else if (reg < 0x0C)
		io_outb(idevp->base + reg - 0x06, data);
	else if (reg < 0x0E)
		io_outb(idevp->ctrl + reg - 0x0A, data);
	else if (reg < 0x16)
		io_outb(idevp->bus_master + reg - 0x0E, data);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(idevp, ATA_REG_CTRL, idevp->nint);
}

static u8 ide_inb(struct ide_device *idevp, u8 reg)
{
	u8 res = 0;

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(idevp, ATA_REG_CTRL, idevp->nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		res = io_inb(idevp->base + reg - 0x00);
	else if (reg < 0x0C)
		res = io_inb(idevp->base + reg - 0x06);
	else if (reg < 0x0E)
		res = io_inb(idevp->ctrl + reg - 0x0A);
	else if (reg < 0x16)
		res = io_inb(idevp->bus_master + reg - 0x0E);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(idevp, ATA_REG_CTRL, idevp->nint);

	return res;
}

static int ide_poll(struct ide_device *idevp)
{
	u8 status;
	int i;

	for (i = 0; i < 5; i++)
		ide_inb(idevp, ATA_REG_STATUS_ALT);

	do {
		status = ide_inb(idevp, ATA_REG_STATUS);
	} while (status & ATA_CMD_BUSY && !(status & ATA_CMD_DDR));

	if (status & ATA_CMD_ERR)
		return -EIO;

	return 0;
}

#ifdef CONFIG_ATAPI
int atapi_out(struct device *devp, const char *buf)
{
	struct ide_device *idevp;
	int res, i;

	idevp = devp->device;
	if (!(idevp->type & IDE_ATAPI))
		return -EINVAL;

	ide_outb(devp->device, ATA_REG_CTRL, 0);

	ide_outb(devp->device, ATA_REG_SELECT, 0xA0 | (idevp->drive << 4));
	sleep(10);

	ide_outb(devp->device, ATA_REG_FEATURES, 0);

	ide_outb(devp->device, ATA_REG_LBA0_LO, ATAPI_SECTOR_SIZE & 0xFF);
	ide_outb(devp->device, ATA_REG_LBA0_MED, ATAPI_SECTOR_SIZE >> 8);

	if ((res = ide_poll(idevp)) < 0)
		return res;

	ide_outb(devp->device, ATA_REG_CMD, ATA_CMD_PACKET);

	for (i = 0; i < 11; i += 2)
		io_outw(idevp->base, ((buf[i] & 0xFF) |
				(buf[i + 1] & 0xFF) << 8));

	return 0;
}

int atapi_in(struct device *devp, char *buf)
{
	struct ide_device *idevp;
	int res;

	idevp = devp->device;
	if (!(idevp->type & IDE_ATAPI))
		return -EINVAL;

	if ((res = ide_poll(idevp)) < 0)
		return res;

	asm volatile ("rep insw" :: "D" (buf), "c" (ATAPI_SECTOR_SIZE / 2),
			"d" (idevp->base) : "memory");

	if ((res = ide_poll(idevp)) < 0)
		return res;

	return 0;
}
#endif

static int ide_config(struct pci_cfg *pcp, u8 ch, u8 drive)
{
	struct device *devp = NULL;
	struct ide_device *idevp;
	u8 lba0_lo, lba0_med, lba0_hi;
	size_t i;
	int res;

	if (!(idevp = kmalloc(sizeof(struct ide_device))))
		return -ENOMEM;

	idevp->bus_master = (pcp->bar_4 & 0xFFFFFFFC) + (ch ? 8 : 0);

	if (ch == 0) {
		idevp->base = pcp->bar_0;
		idevp->ctrl = pcp->bar_1;

		if (drive == 0) {
			ide_outb(idevp, ATA_REG_CTRL, 0x02);

			pcp->int_line = 14;
			/* irq_unmask(pcp->int_line); */
		}
	} else if (ch == 1) {
		idevp->base = pcp->bar_2;
		idevp->ctrl = pcp->bar_3;

		if (drive == 0) {
			ide_outb(idevp, ATA_REG_CTRL, 0x02);

			pcp->int_line = 15;
			/* irq_unmask(pcp->int_line); */
		}
	}

	ide_outb(idevp, ATA_REG_SELECT, 0xA0 | (drive << 4));
	sleep(10);

	/* FIXME Detection fails on VirtualBox (and possibly also on real
	 * hardware
	 */
	if ((res = ide_poll(idevp)) < 0)
		goto err;

	ide_outb(idevp, ATA_REG_CMD, ATA_CMD_IDENT);
	sleep(10);

	lba0_lo = ide_inb(idevp, ATA_REG_LBA0_LO);
	lba0_med = ide_inb(idevp, ATA_REG_LBA0_MED);
	lba0_hi = ide_inb(idevp, ATA_REG_LBA0_HI);

#ifdef CONFIG_ATA
	if ((lba0_lo == 0x01 && lba0_med == 0 && lba0_hi == 0)) {
		idevp->type = IDE_ATA;

		if ((res = device_reg(&ide_driver, &devp, 0)) < 0)
			goto err;
		/* if ((res = ata_probe(devp)) < 0)
			goto err; */
	} else
#endif
#ifdef CONFIG_ATAPI
	if (lba0_lo == 0x01 && lba0_med == 0x14 && lba0_hi == 0xEB) {
		idevp->type = IDE_ATAPI;

		if ((res = device_reg(&ide_driver, &devp, 0)) < 0)
			goto err;
		if ((res = atapi_probe(devp)) < 0)
			goto err;

		ide_outb(idevp, ATA_REG_CMD, ATAPI_CMD_IDENT);
		sleep(10);
	} else
#endif
	{
		goto err;
	}

	for (i = 0; i < sizeof(struct ata_ident) / 2; i++)
		((u16 *) &idevp->ident)[i] = io_inw(idevp->base);

	for (i = 0; i < sizeof(idevp->ident.model) - 1; i += 2) {
		char c = idevp->ident.model[i];
		idevp->ident.model[i] = idevp->ident.model[i + 1];
		idevp->ident.model[i + 1] = c;
	}

	devp->name = strtrm(idevp->ident.model);;

	devp->bus = pcp;
	devp->device = idevp;

	dprintf(KP_CON "%u: %s, %s drive @ IRQ %u\n",
			MINOR(devp->dev), devp->name,
			(idevp->type ? "ATAPI": "ATA"), pcp->int_line);

	return 0;

err:
	if (devp)
		device_unreg(devp);
	kfree(idevp);

	return res;
}

static int ide_probe(struct device *devp)
{
	u8 ch, drive;
	int res;

	/* io_outb(IDE_CH1_CMD, 0x04);
	io_outb(IDE_CH1_CMD, 0x04);
	io_outb(IDE_CH1_CMD, 0x04);
	io_outb(IDE_CH1_CMD, 0x04);
	io_outb(IDE_CH1_CMD, 0x00); */

	((struct pci_cfg *) devp->bus)->bar_0 = IDE_CH0_IO;
	((struct pci_cfg *) devp->bus)->bar_1 = IDE_CH0_CMD;
	((struct pci_cfg *) devp->bus)->bar_2 = IDE_CH1_IO;
	((struct pci_cfg *) devp->bus)->bar_3 = IDE_CH1_CMD;

	for (ch = 0; ch < IDE_CHANNELS; ch++) {
		for (drive = 0; drive < IDE_DRIVES; drive++)
			if ((res = ide_config(devp->bus, ch, drive)) < 0)
				return res;
	}

	return 0;
}

static void ide_fini(struct device *devp)
{
	(void) devp;

	/* TODO */
}

static const struct pci_id ide_pci_ids[] = {
	PCI_ID(0x8086, PCI_ID_ANY, 0x7010, PCI_ID_ANY, 0x01, 0x01, PCI_ID_ANY),
	PCI_ID(0x8086, PCI_ID_ANY, 0x269E, PCI_ID_ANY, 0x01, 0x01, PCI_ID_ANY),
	PCI_ID(0x8086, PCI_ID_ANY, 0x7111, PCI_ID_ANY, 0x01, 0x01, PCI_ID_ANY)
};

static struct driver ide_driver = {
	.name	= devname,
	.major	= MAJOR_DSK,

	.busid	= BUS_PCI,
	.bus	= &ide_pci_ids,

	.probe	= &ide_probe,
	.fini	= &ide_fini
};

int ide_init(void)
{
	return driver_reg(&ide_driver);
}

void ide_exit(void)
{
	driver_unreg(&ide_driver);
}

MODULE(ide, &ide_init, &ide_exit);
MODULE_DEP(pci);
