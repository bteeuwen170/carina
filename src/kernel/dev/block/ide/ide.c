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

#include <dev.h>
#include <kernel.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <timer/pit.h>

#include <stdlib.h>
#include <string.h>

#include "ide.h"

static const char devname[] = "ide";

struct ata_channel ide_channels[IDE_CHANNELS];
struct ata_dev ide_devices[IDE_CHANNELS * IDE_DRIVES];

static int ide_minor_last = -1;

static const struct pci_dev_id ide_ids[] = {
	PCI_DEV_ID(0x8086, 0x7010, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x269E, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x7111, PCI_ANY_ID, PCI_ANY_ID, 0x01, 0x01, 0)
};

void ide_outb(u8 ch, u8 reg, u8 data)
{
	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, ide_channels[ch].nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		io_outb(ide_channels[ch].base + reg - 0x00, data);
	else if (reg < 0x0C)
		io_outb(ide_channels[ch].base + reg - 0x06, data);
	else if (reg < 0x0E)
		io_outb(ide_channels[ch].ctrl + reg - 0x0A, data);
	else if (reg < 0x16)
		io_outb(ide_channels[ch].bus_master + reg - 0x0E, data);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, ide_channels[ch].nint);
}

u8 ide_inb(u8 ch, u8 reg)
{
	u8 res;

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, ide_channels[ch].nint | ATA_CMD_BUSY);

	if (reg < 0x08)
		res = io_inb(ide_channels[ch].base + reg - 0x00);
	else if (reg < 0x0C)
		res = io_inb(ide_channels[ch].base + reg - 0x06);
	else if (reg < 0x0E)
		res = io_inb(ide_channels[ch].ctrl + reg - 0x0A);
	else if (reg < 0x16)
		res = io_inb(ide_channels[ch].bus_master + reg - 0x0E);

	if (reg > ATA_REG_CMD && reg < ATA_REG_CTRL)
		ide_outb(ch, ATA_REG_CTRL, ide_channels[ch].nint);

	return res;
}

int ide_poll(u8 ch)
{
	u8 i, status;

	for (i = 0; i < 4; i++)
		ide_inb(ch, ATA_REG_STATUS_ALT);

	while ((status = ide_inb(ch, ATA_REG_STATUS)) & ATA_CMD_BUSY);

	if (status & ATA_CMD_ERR)
		return 1;

	return 0;
}

static void ide_config(struct pci_dev *card, u8 ch, u8 drv)
{
	struct ata_dev *dev;
	char *str;
	u32 i, status;

	/* FIXME XXX XXX Something is very wrong here! XXX XXX FIXME */
	if (ch == 0) return;
	/* FIXME XXX XXX Something is very wrong here! XXX XXX FIXME */

	dev = &ide_devices[++ide_minor_last];

	dev->ch = ch;
	dev->drv = drv;

	if (ch == 0) {
		irq_unmask(IRQ_ATA0);
		ide_channels[ch].base = card->cfg->bar_0;
		ide_channels[ch].ctrl = card->cfg->bar_1;

		card->cfg->int_line = 14;
	} else if (ch == 1) {
		irq_unmask(IRQ_ATA1);
		ide_channels[ch].base = card->cfg->bar_2;
		ide_channels[ch].ctrl = card->cfg->bar_3;

		card->cfg->int_line = 15;
	}

	ide_channels[ch].bus_master = (card->cfg->bar_4 & 0xFFFFFFFC) +
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
		u8 lba0 = ide_inb(ch, ATA_REG_LBA0);
		u8 lba1 = ide_inb(ch, ATA_REG_LBA1);
		u8 lba2 = ide_inb(ch, ATA_REG_LBA2);
		u8 packet[12];
		u8 buf[2];

		if (lba0 == 0x01 && lba1 == 0x00 && lba2 == 0x00) {
			dev->type = ATA_DEV_TYPE_ATA;
		} else if (lba0 == 0x01 && lba1 == 0x14 && lba2 == 0xEB) {
			dev->type = ATA_DEV_TYPE_ATAPI;

			ide_outb(ch, ATA_REG_CMD, ATAPI_CMD_IDENT);
			sleep(1);

#if 0
			/* Set PIO mode */
			ide_outb(dev->ch, ATA_REG_FEATURES, 0);

			/* Set buffer size */
			ide_outb(dev->ch, ATA_REG_LBA1, ATAPI_SECTOR_SIZE >> 8);
			ide_outb(dev->ch, ATA_REG_LBA2, ATAPI_SECTOR_SIZE >> 8);

			packet[0] = ATAPI_CMD_READ_CAPACITY;
			packet[1] = 0;
			packet[2] = 0;
			packet[3] = 0;
			packet[4] = 0;
			packet[5] = 0;
			packet[6] = 0;
			packet[7] = 0;
			packet[8] = 0;
			packet[9] = 0;
			packet[10] = 0;
			packet[11] = 0;

			/* Send the packet */
			ide_outb(dev->ch, ATA_REG_CMD, ATA_CMD_PACKET);

			if (ide_poll(dev->ch) != 0)
				return -1;

			for (i = 0; i < 11; i += 2)
				io_outw(ide_channels[dev->ch].base, ((packet[i] & 0xFF) |
							(packet[i + 1] & 0xFF) << 8));

			for (i = 0; i < n / ATAPI_SECTOR_SIZE; i++) {
				if (ide_poll(dev->ch) != 0)
					return -1;

				asm volatile ("rep insw" :: "D" (buf), "c" (8 / 2),
						"d" (ide_channels[dev->ch].base) : "memory");
			}

			while (ide_inb(dev->ch, ATA_REG_STATUS) & (ATA_CMD_BUSY | ATA_CMD_DDR));
#endif
		} else {
			goto err;
		}
	} else {
		goto err;
	}

	for (i = 0; i < sizeof(struct ata_ident); i++)
		((u16 *) &dev->ident)[i] = io_inw(ide_channels[ch].base);

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

	dprintf("%s, %s drive @ IRQ %u\n",
			strtrm(dev->ident.model), (dev->type ? "ATAPI": "ATA"),
			card->cfg->int_line);
	dprintf(KP_CON "%u sectors (%u MB)\n",
			dev->size, dev->size * ATA_SECTOR_SIZE / 1024 / 1024);

	/* device_reg(DEV((dev->type ? MAJOR_OPTICAL: MAJOR_DISK), ide_minor_last)); */

	return;

	/* TODO Provide error information */
err:
	dprintf(KP_ERR "drive error (%u, %u)\n", drv, ch);

ret:
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
	dprintf(KP_ERR "err\n");
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
