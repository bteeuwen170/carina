/*
 *
 * Elarix
 * src/kernel/dev/block/atapi.c
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
#include <dev.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <ioctl.h>
#include <kernel.h>
#include <module.h>

static const char devname[] = "atapi";

static int atapi_read(struct file *fp, char *buf, off_t off, size_t n)
{
	struct device *devp;
	char packet[12];
	int res;

	if (!(devp = device_get(fp->ip->rdev)))
		return -ENODEV;

	if (n != 1)
		return -EINVAL;

	packet[0] = ATAPI_CMD_READ;
	packet[1] = 0;
	packet[2] = (off >> 24) & 0xFF;
	packet[3] = (off >> 16) & 0xFF;
	packet[4] = (off >> 8) & 0xFF;
	packet[5] = off & 0xFF;
	packet[6] = 0;
	packet[7] = 0;
	packet[8] = 0;
	packet[9] = n;
	packet[10] = 0;
	packet[11] = 0;

	if ((res = atapi_out(devp, packet)) < 0)
		return res;

	return atapi_in(devp, buf);
}

static int atapi_ioctl(struct file *fp, unsigned int cmd, va_list args)
{
	struct device *devp;
	char packet[12];
	(void) args;

	if (cmd != IO_EJECT)
		return -EINVAL;

	if (!(devp = device_get(fp->ip->rdev)))
		return -ENODEV;

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

	return atapi_out(devp, packet);
}

#if 0
int atapi_identify()
{
#if 0
	ide_outb(dev->ch, ATA_REG_FEATURES, 0);

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
}
#endif

static struct file_ops atapi_file_ops = {
	.read	= &atapi_read,
	.ioctl	= &atapi_ioctl
};

int atapi_probe(struct device *devp)
{
	devp->op = &atapi_file_ops;

	return 0;
}

int atapi_init(void)
{
	return 0;
}

void atapi_exit(void) { }

MODULE(atapi, &atapi_init, &atapi_exit);
MODULE_DEP(ide);
