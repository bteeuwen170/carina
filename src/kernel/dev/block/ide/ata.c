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

/*static void ide_wait(struct ata_dev *dev)
{
	int i;

	for (i = 0; i < 4; i++)
		io_inb(dev->base + ATA_REG_STATUS_ALT);
}*/

static void ide_atapi_eject(u8 drive)
{

}

static void ide_config(struct pci_dev *card, u8 ch, u8 drv)
{
	struct ata_dev *device;
	u8 buf[2048], status;

	device = kmalloc(sizeof(struct ata_dev));

	device->channel = ch;
	device->drive = drv;

	if (ch) {
		irq_unmask(IRQ_ATA1);
		channels[ch].base = (card->cfg->base_adr_2 & 0xFFFFFFFC) +
				ATA_CH1_IO * (!card->cfg->base_adr_2);
		channels[ch].ctrl = (card->cfg->base_adr_3 & 0xFFFFFFFC) +
				ATA_CH1_CMD * (!card->cfg->base_adr_3);
	} else {
		irq_unmask(IRQ_ATA0);
		channels[ch].base = (card->cfg->base_adr_0 & 0xFFFFFFFC) +
				ATA_CH0_IO * (!card->cfg->base_adr_0);
		channels[ch].ctrl = (card->cfg->base_adr_1 & 0xFFFFFFFC) +
				ATA_CH0_CMD * (!card->cfg->base_adr_1);
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

	status = ide_inb(ch, ATA_REG_STATUS);

	/* FIXME Buggy */
	do {
		if (!status)
			return;

		status = ide_inb(ch, ATA_REG_STATUS);

		/* XXX Directly from OSDEV, change and understand */
		if (status & ATA_CMD_ERR) {
			u8 cl = ide_inb(ch, card->cfg->base_adr_1);
			u8 cd = ide_inb(ch, card->cfg->base_adr_2);

			kprintf(0, 0, "%u %u\n", cl, cd);
			if ((cl == 0x14 && cd == 0xEB) ||
					(cl == 0x69 && cd == 0x96))
				kprintf(0, 0, "ATAPI!\n");
			else
				return;

			ide_outb(ch, ATA_REG_CMD, ATAPI_CMD_IDENT);

			break;
		} else if (status & ATA_CMD_DDR) {
			break;
		}
	} while (status & ATA_CMD_BUSY);

	//if (!(device->features & 0x40))
	//	goto err;

	kprintf(KP_INFO, devname, "initialized");

	return;

	/* TODO Provide error information */
err:
	kprintf(KP_ERR, devname, "err\n");
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
	/* TODO Register by class ID */
}
