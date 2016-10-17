/*
 *
 * Carina
 * src/kernel/dev/block/ide/ata.c
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

#include <stdlib.h>
#include <string.h>
#include <cpu.h>
#include <pci/pci.h>
#include <timer/pit.h>
#include <kernel/print.h>

#include "ata.h"

static char *devname = "ide";

struct ata_channel channels[ATA_CHANNELS];
struct ata_dev devices[ATA_CHANNELS * ATA_DRIVES];

static void ide_outb(u8 ch, u8 reg, u8 data)
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

static u8 ide_inb(u8 ch, u8 reg)
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

/*static int ata_read()
{

}

static int ata_write()
{

}*/

/* FIXME Broken */
static int atapi_read(struct ata_dev *dev, u64 addr, u16 ss, u32 off, u8 *buf)
{
	u32 status, i;
	u16 data;
	u8 packet[12];

	if (dev->type != ATA_DEV_TYPE_ATAPI)
		return -1;

	/* Enable interrupts */
	ide_outb(dev->ch, ATA_REG_CTRL, 0x00);

	/* Select the drive */
	ide_outb(dev->ch, ATA_REG_DRVSEL, 0xA0 | (dev->drv << 4));
	sleep(1);

	/* Set PIO mode */
	ide_outb(dev->ch, ATA_REG_FEATURES, 0);

	/* Set buffer size */
	ide_outb(dev->ch, ATA_REG_LBA1, ATAPI_SECTOR_SIZE & 0xFF);
	ide_outb(dev->ch, ATA_REG_LBA2, ATAPI_SECTOR_SIZE >> 8);

	/* Send the packet */
	ide_outb(dev->ch, ATA_REG_CMD, ATA_CMD_PACKET);

	while (status & ATA_CMD_BUSY)
		status = ide_inb(dev->ch, ATA_REG_STATUS);

	if (status & ATA_CMD_ERR)
		return -1;

	packet[0] = ATAPI_CMD_READ;
	packet[1] = 0;
	packet[2] = (addr >> 24) & 0xFF;
	packet[3] = (addr >> 16) & 0xFF;
	packet[4] = (addr >> 8) & 0xFF;
	packet[5] = addr & 0xFF;
	packet[6] = 0;
	packet[7] = 0;
	packet[8] = 0;
	packet[9] = 1; /* Number of sectors to write */
	packet[10] = 0;
	packet[11] = 0;

	for (i = 0; i < 11; i += 2)
		io_outw(channels[dev->ch].base, ((packet[i] & 0xFF) |
					(packet[i + 1] & 0xFF) << 8));

	/* FIXME Not right from here onwards */
	while (status & ATA_CMD_BUSY)
		status = ide_inb(channels[dev->ch].base, ATA_REG_STATUS);

	if (status & ATA_CMD_ERR)
		return -1;

	for (i = 0; i < 4; i++)
		buf[i] = io_inw(channels[dev->ch].base);

	return 0;
}

/* TODO Using ioctl */
static int ide_eject(struct ata_dev *dev)
{
	u32 status, i;
	u8 packet[12];

	if (dev->type != ATA_DEV_TYPE_ATAPI)
		return -1;

	/* Enable interrupts */
	ide_outb(dev->ch, ATA_REG_CTRL, 0x00);

	/* Select the drive */
	ide_outb(dev->ch, ATA_REG_DRVSEL, 0xA0 | (dev->drv << 4));
	sleep(1);

	/* Send the packet */
	ide_outb(dev->ch, ATA_REG_CMD, ATA_CMD_PACKET);

	while (status & ATA_CMD_BUSY)
		status = ide_inb(dev->ch, ATA_REG_STATUS);

	if (status & ATA_CMD_ERR)
		return -1;

	packet[0] = ATAPI_CMD_EJECT;
	packet[1] = 0;
	packet[2] = 0;
	packet[3] = 0;
	packet[4] = 0x02;
	packet[5] = 0;
	packet[6] = 0;
	packet[7] = 0;
	packet[8] = 0;
	packet[9] = 0;
	packet[10] = 0;
	packet[11] = 0;

	for (i = 0; i < 11; i += 2)
		io_outw(channels[dev->ch].base, ((packet[i] & 0xFF) |
					(packet[i + 1] & 0xFF) << 8));

	while (status & ATA_CMD_BUSY)
		status = ide_inb(channels[dev->ch].base, ATA_REG_STATUS);

	if (status & ATA_CMD_ERR)
		return -1;

	return 0;
}

static void ide_config(struct pci_dev *card, u8 ch, u8 drv)
{
	struct ata_dev *dev;
	u32 i, status;

	dev = kmalloc(sizeof(struct ata_dev));

	dev->ch = ch;
	dev->drv = drv;

	/* Primary channel */
	if (ch) {
		irq_unmask(IRQ_ATA1);
		channels[ch].base = card->cfg->base_adr_0;
		channels[ch].ctrl = card->cfg->base_adr_1;
		/*channels[ch].base = (card->cfg->base_adr_2 & 0xFFFFFFFC) +
				ATA_CH1_IO * (!card->cfg->base_adr_2);
		channels[ch].ctrl = (card->cfg->base_adr_3 & 0xFFFFFFFC) +
				ATA_CH1_CMD * (!card->cfg->base_adr_3);*/
	/* Secondary channel */
	} else {
		irq_unmask(IRQ_ATA0);
		channels[ch].base = card->cfg->base_adr_2;
		channels[ch].ctrl = card->cfg->base_adr_3;
		/*channels[ch].base = (card->cfg->base_adr_0 & 0xFFFFFFFC) +
				ATA_CH0_IO * (!card->cfg->base_adr_0);
		channels[ch].ctrl = (card->cfg->base_adr_1 & 0xFFFFFFFC) +
				ATA_CH0_CMD * (!card->cfg->base_adr_1);*/
	}

	channels[ch].bus_master = (card->cfg->base_adr_4 & 0xFFFFFFFC) +
			((ch) ? 0 : 8);

	/* Disable interrupts */
	ide_outb(ch, ATA_REG_CTRL, 0x02);

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
	/* FIXME Corrupted strings */
	char *str = ((char *) &dev->ident.model);
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

	kprintf(KP_INFO, devname, "%s, %s drive @ TODO on IRQ %u\n",
			strtrm(dev->ident.model), (dev->type ? "ATAPI": "ATA"),
			card->cfg->int_line);
	kprintf(KP_INFO, devname, "%u sectors (%u MB)\n",
			dev->size, dev->size * ATA_SECTOR_SIZE / 1024 / 1024);

	/* XXX TEMPORARY XXX */
	if (dev->type)
		ide_eject(dev);

	return;

	/* TODO Provide error information */
err:
	kprintf(KP_ERR, devname, "drive error (%u, %u)\n", drv, ch);

ret:
	kfree(dev);

	return;

}

static int pci_handler(struct pci_dev *card)
{
	u16 ch, drv;

	if (card->cfg->base_adr_0 > 1)
		goto err;

	if (card->cfg->base_adr_1 > 1)
		goto err;

	//io_outb(ATA_CH1_CMD, 0x04);
	//io_outb(ATA_CH1_CMD, 0x04);
	//io_outb(ATA_CH1_CMD, 0x04);
	//io_outb(ATA_CH1_CMD, 0x04);
	//io_outb(ATA_CH1_CMD, 0x00);

	card->cfg->base_adr_0 = ATA_CH0_IO;
	card->cfg->base_adr_1 = ATA_CH0_CMD;
	card->cfg->base_adr_2 = ATA_CH1_IO;
	card->cfg->base_adr_3 = ATA_CH1_CMD;
	
	for (ch = 0; ch < ATA_CHANNELS; ch++) {
		for (drv = 0; drv < ATA_DRIVES; drv++) {
			ide_config(card, ch, drv);
		}
	}

	return 0;

	/* TODO Provide error information */
err:
	kprintf(KP_ERR, devname, "err\n");
	return 1;
}

void ide_reghandler(void)
{
	pci_reghandler(0x8086, 0x7010, 1, &pci_handler);
	pci_reghandler(0x8086, 0x269E, 1, &pci_handler);
	pci_reghandler(0x8086, 0x7111, 1, &pci_handler);
	/* TODO Register by class ID */
}
