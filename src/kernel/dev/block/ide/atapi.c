/*
 *
 * Elarix
 * src/kernel/dev/block/ide/atapi.c
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

#include <errno.h>
#include <fs.h>
#include <kernel.h>
#include <module.h>

#include <timer/pit.h>

#include "ide.h"

static const char devname[] = "opt";

/* static int atapi_read(struct ata_dev *dev, u64 addr, u16 ss, u32 off, u8 *buf) */
static int atapi_read(struct file *fp, char *buf, off_t off, size_t n)
{
	u32 status, i;
	u8 packet[12];

	/* if (fp->dep->ip->dev->type != ATA_DEV_TYPE_ATAPI)
		return -EINVAL; */

	if (n != ATAPI_SECTOR_SIZE)
		return -EINVAL;

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

static struct file_ops atapi_file_ops = {
	.read	= &atapi_read
};

int atapi_init(void)
{
	int res;

	if ((res = dev_reg(MAJOR_OPTICAL, devname, &atapi_file_ops)) < 0)
		kprintf("%s: unable to register atapi driver (%d)",
				devname, res);

	return res;
}

void atapi_exit(void)
{
	/* TODO */
}

MODULE(atapi, &atapi_init, &atapi_exit);
MODULE_DEP(ide);
