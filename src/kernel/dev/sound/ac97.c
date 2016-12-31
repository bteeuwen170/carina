/*
 *
 * Elarix
 * src/kernel/dev/sound/ac97.c
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

#include <errno.h>
#include <kernel.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <timer/pit.h>

#include <stdlib.h>

#include "ac97.h"
#include "snd.h"

static const char devname[] = "ac97";

static const struct pci_dev_id ac97_ids[] = {
	PCI_DEV_ID(0x8086, 0x2415, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x2425, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x2445, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0),
	PCI_DEV_ID(0x8086, 0x24C5, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0)
};

struct buffer {
	u32	addr;
	u16	len;
	u16	reserved:14;
	u8	bup:1;
	u8	ioc:1;
} __attribute__ ((packed));

struct ac97_dev {
	u16	nambar;
	u16	nabmbar;
	/* u16	sample_rate; */

	struct buffer *buf;
	u32	index;
};

/* TEMP */
struct ac97_dev deva;
struct ac97_dev *dev = &deva;

static void buffer_fill(void *data, u32 n, u32 off)
{
	dev->buf[n].addr = (uintptr_t) data + (off * 32 * 2);
	dev->buf[n].len = 32;
	dev->buf[n].bup = 0;
	dev->buf[n].ioc = 1;
}

static int int_handler(struct int_stack *regs)
{
	u16 status;
	u32 buf;
	(void) regs;

	status = io_inw(dev->nabmbar + 0x16);

	if (status & 0b100) {
		io_outw(dev->nabmbar + 0x16, 0b100);
	} else if (status & 0b1000) {
		buf = (dev->index + 1) % 32;
		buffer_fill(snd_wav, buf, ++dev->index);

		io_outb(dev->nabmbar + 0x15, 32);
		io_outw(dev->nabmbar + 0x16, 0b1000);

		/* dprintf(devname, KP_DBG "sc: %u\n",
				io_inw(dev->nabmbar + 0x16)); */
	} else {
		asm volatile ("cli");
	}

	return 1;
}

static void volume_set(u8 volume)
{
	io_outw(dev->nambar + 0x02, (volume << 8) | volume);
	io_outw(dev->nambar + 0x18, (volume << 8) | volume);

	sleep(10);
}

void ac97_play(void)
{
	int i;

	dev->buf = kmalloc(sizeof(struct buffer) * 32);

	dev->index = 32 - 1;

	/* TODO Check file size */
	for (i = 0; i < 32; i++)
		buffer_fill(snd_wav, i, i);

	io_outd(dev->nabmbar + 0x10, (uintptr_t) dev->buf);

	dprintf(devname, KP_DBG "wav playing\n");

	/* X_LVI */ io_outb(dev->nabmbar + 0x15, 32);
	/* XXX Use LVBIE or CELV for interrupt? */
	/* X_CR */ io_outb(dev->nabmbar + 0x1B, 0b11001);
}

static int ac97_probe(struct pci_dev *card)
{
	int res;

	res = irq_handler_reg(card->cfg->int_line, &int_handler);

	if (res < 0)
		goto err;

	dev->nambar = card->cfg->bar_0 & ~1;
	dev->nabmbar = card->cfg->bar_1 & ~1;

	if (dev->nambar <= 0 || dev->nabmbar <= 0) {
		res = -EBUSY;
		goto err;
	}

	/* Set PIO control */
	pci_outd(card, 4, 5);

	/* Reset */
	io_outw(dev->nambar + 0x00, 0x01);
	sleep(20);

	/* Register reset */
	/* X_CR */ io_outb(dev->nabmbar + 0x1B, 0x02);

	/* Check if sample rate is set out of the box */
	if (io_inw(dev->nambar + 0x28) & 1) {
		io_outw(dev->nambar + 0x2A, io_inw(dev->nambar + 0x2A) | 0x01);
		sleep(10);
		io_outw(dev->nambar + 0x2C, 48000);
		io_outw(dev->nambar + 0x32, 48000);
	}

	volume_set(1);

	/* TODO Detect vendor */
	dprintf(devname, "initialized, %u Hz @ IRQ %u\n",
			io_inw(dev->nambar + 0x2C), card->cfg->int_line);

	return res;

err:
	irq_handler_unreg(card->cfg->int_line);

	dprintf(devname, KP_ERR "unable to intialize ac97 card\n");

	return res;
}

static void ac97_fini(struct pci_dev *card)
{
	(void) card;
	/* TODO */
}

static struct pci_driver ac97_driver = {
	.name	= devname,

	.ids	= ac97_ids,

	.probe	= &ac97_probe,
	.fini	= &ac97_fini
};

int ac97_init(void)
{
	return pci_driver_reg(&ac97_driver);
}

void ac97_exit(void)
{
	pci_driver_unreg(&ac97_driver);
}

MODULE(ac97, &ac97_init, &ac97_exit);
MODULE_DEP(pci);
