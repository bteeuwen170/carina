/*
 *
 * Elarix
 * src/kernel/dev/block/ide/ide.c
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

#include <fs.h>
#include <kernel.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <timer/pit.h>

#include <stdlib.h>
#include <string.h>

#include "ide.h"

static const char devname[] = "ide";

static struct ata_channel channels[IDE_CHANNELS];
static struct ata_dev devices[IDE_CHANNELS * IDE_DRIVES];

static const struct pci_dev_id ide_ids[] = {
	PCI_DEV_ID(0x8086, 0x7010, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x269E, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x7111, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0)
};

void ide_outb(u8 ch, u8 reg, u8 data)
{
	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, channels[ch].nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		io_outb(channels[ch].base + reg - 0x00, data);
	else if (reg < 0x0C)
		io_outb(channels[ch].base + reg - 0x06, data);
	else if (reg < 0x0E)
		io_outb(channels[ch].ctrl + reg - 0x0A, data);
	else if (reg < 0x16)
		io_outb(channels[ch].bus_master + reg - 0x0E, data);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, channels[ch].nint);
}

u8 ide_inb(u8 ch, u8 reg)
{
	u8 res;

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, channels[ch].nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		res = io_inb(channels[ch].base + reg - 0x00);
	else if (reg < 0x0C)
		res = io_inb(channels[ch].base + reg - 0x06);
	else if (reg < 0x0E)
		res = io_inb(channels[ch].ctrl + reg - 0x0A);
	else if (reg < 0x16)
		res = io_inb(channels[ch].bus_master + reg - 0x0E);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, channels[ch].nint);

	return res;
}

static void ide_config(struct pci_dev *card, u8 ch, u8 drv)
{
	struct ata_dev *dev;
	char *str;
	u32 i, status;

	dev = kmalloc(sizeof(struct ata_dev));

	if (!dev)
		goto err;

	dev->ch = ch;
	dev->drv = drv;

	/* Primary channel */
	if (ch == 0) {
		irq_unmask(IRQ_ATA0);
		channels[ch].base = card->cfg->bar_0;
		channels[ch].ctrl = card->cfg->bar_1;

		card->cfg->int_line = 14;
	/* Secondary channel */
	} else if (ch == 1) {
		irq_unmask(IRQ_ATA1);
		channels[ch].base = card->cfg->bar_2;
		channels[ch].ctrl = card->cfg->bar_3;

		card->cfg->int_line = 15;
	}

	channels[ch].bus_master = (card->cfg->bar_4 & 0xFFFFFFFC) +
			((ch) ? 0 : 8);

	/* Select the drive */
	ide_outb(ch, ATA_REG_DRVSEL, 0xA0 | (drv << 4));
	sleep(1);

	/* Identify */
	ide_outb(ch, ATA_REG_CMD, ATA_CMD_IDENT);
	sleep(1);

	/* Check if device exists */
	status = ide_inb(ch, ATA_REG_STATUS);
	if (!status)
		goto ret;

	/* XXX Safe? */
	while (status & ATA_CMD_BUSY)
		status = ide_inb(ch, ATA_REG_STATUS);

	if (status & ATA_CMD_ERR || status & ATA_CMD_DDR) {
		u8 cl = ide_inb(ch, ATA_REG_LBA1);
		u8 cd = ide_inb(ch, ATA_REG_LBA2);

		if ((cl == 0x14 && cd == 0xEB) ||
				(cl == 0x69 && cd == 0x96))
			dev->type = ATA_DEV_TYPE_ATAPI;
		else if ((cl == 0x00 && cd == 0x00) ||
				(cl == 0x3C && ch == 0xC3))
			dev->type = ATA_DEV_TYPE_ATA;
		else
			goto err;

		ide_outb(ch, ATA_REG_CMD, ATAPI_CMD_IDENT);
		sleep(1);
	} else {
		goto err;
	}

	for (i = 0; i < sizeof(struct ata_ident); i++)
		((u16 *) &dev->ident)[i] = io_inw(channels[ch].base);

	/* This wasn't documented ANYWHERE */
	str = ((char *) &dev->ident.model);
	for (i = 0; i < sizeof(dev->ident.model) - 1; i += 2) {
		char cur = str[i];
		str[i] = str[i + 1];
		str[i + 1] = cur;
	}

	if (dev->ident.cmdset.lba48 && dev->ident.cmdset_active.lba48)
		dev->size = dev->ident.lba48_max;
	else if (dev->ident.features.lba28)
		dev->size = dev->ident.lba28_max;
	else
		goto ret;

	dprintf(devname, "%s, %s drive @ IRQ %u\n",
			strtrm(dev->ident.model), (dev->type ? "ATAPI": "ATA"),
			card->cfg->int_line);
	dprintf(devname, KP_CON "%u sectors (%u MB)\n",
			dev->size, dev->size * ATA_SECTOR_SIZE / 1024 / 1024);

	return;

	/* TODO Provide error information */
err:
	dprintf(devname, KP_ERR "drive error (%u, %u)\n", drv, ch);

ret:
	kfree(dev);

	return;

}

static int ide_probe(struct pci_dev *card)
{
	u16 ch, drv;

	if (card->cfg->bar_0 > 1)
		goto err;

	if (card->cfg->bar_1 > 1)
		goto err;

	/* io_outb(ATA_CH1_CMD, 0x04);
	io_outb(ATA_CH1_CMD, 0x04);
	io_outb(ATA_CH1_CMD, 0x04);
	io_outb(ATA_CH1_CMD, 0x04);
	io_outb(ATA_CH1_CMD, 0x00); */

	card->cfg->bar_0 = ATA_CH0_IO;
	card->cfg->bar_1 = ATA_CH0_CMD;
	card->cfg->bar_2 = ATA_CH1_IO;
	card->cfg->bar_3 = ATA_CH1_CMD;

	for (ch = 0; ch < IDE_CHANNELS; ch++) {
		/* Disable interrupts */
		ide_outb(ch, ATA_REG_CTRL, 0x02);

		for (drv = 0; drv < IDE_DRIVES; drv++) {
			ide_config(card, ch, drv);
		}
	}

	return 0;

	/* TODO Provide error information */
err:
	dprintf(devname, KP_ERR "err\n");
	return -1;
}

static void ide_fini(struct pci_dev *card)
{
	(void) card;
	/* TODO */
}

static struct pci_driver ide_driver = {
	.name	= devname,

	.ids	= ide_ids,

	.probe	= &ide_probe,
	.fini	= &ide_fini
};

int ide_init(void)
{
	return pci_driver_reg(&ide_driver);
}

void ide_exit(void)
{
	pci_driver_unreg(&ide_driver);
}

MODULE(ide, &ide_init, &ide_exit);
MODULE_DEP(pci);
