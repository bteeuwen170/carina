/*
 *
 * Elara
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
/* FIXME This is broken */

#include <print.h>

#include <asm/cpu.h>

#include <pci/pci.h>
#include <timer/pit.h>

//#include <stdint.h> /* TEMP */
#include <stdlib.h>
#include <string.h> /* TEMP */

#include "ac97.h"
#include "snd.h"

static const char devname[] = "ac97";

static const struct pci_dev_id dev_ids[] = {
	{ PCI_DEVICE(0x8086, 0x2415, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0) },
	{ PCI_DEVICE(0x8086, 0x2425, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0) },
	{ PCI_DEVICE(0x8086, 0x2445, PCI_ANY_ID, PCI_ANY_ID, 0x04, 0x01, 0) }
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
//	u16	sample_rate;

	struct buffer *buf;
	u32	index;
	u32	prev;
};

/* TEMP */
struct ac97_dev deva;
struct ac97_dev *dev = &deva;

static void buffer_fill(void *data, u32 n, u32 off)
{
	/* TEMP 400 to skip header */
	dev->buf[n].addr = (intptr_t) data + 400 + off;
	dev->buf[n].len = 32;
	dev->buf[n].bup = 0;
	dev->buf[n].ioc = 1;
}

static void int_handler(struct int_stack *regs)
{
	(void) regs;

	kprintf(KP_DBG, devname, "!"); //TEMPORARY

	u32 curbuf = (dev->prev + 1) % 32;

	if (!curbuf || io_inw(dev->nabmbar + 0x18) < 1)
		io_outw(dev->nabmbar + 0x15, 32);

	io_outw(dev->nabmbar + 0x16, 0x08);

	buffer_fill(snd_wav, dev->prev, ++dev->index);

	dev->prev = curbuf;
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
	dev->prev = 0;

	/* TODO Check file size */
	for (i = 0; i < 32; i++)
		buffer_fill(snd_wav, i, i);

	io_outd(dev->nabmbar + 0x10, (intptr_t) dev->buf);

	/* X_LVI */ io_outb(dev->nabmbar + 0x15, 32);
	/* XXX Use LVBIE or CELV for interrupt? */
	/* X_CR */ io_outb(dev->nabmbar + 0x1B, 0x19);
	/* Now set: bit 4 (IOCE), bit 3 (FEIE) and bit 0 (RPBM) */

	kprintf(KP_DBG, devname, "sr: %u\n", io_inw(dev->nabmbar + 0x16));

	kprintf(KP_DBG, devname, "wav playing\n");
}

static int ac97_probe(struct pci_dev *card)
{
	dev->nambar = card->cfg->bar_0 & ~1;
	dev->nabmbar = card->cfg->bar_1 & ~1;

	/* Set PIO control */
	pci_outd(card, 4, 5);

	if (dev->nambar <= 0 || dev->nabmbar <= 0)
		goto err;

	sleep(20);

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

	volume_set(0);

	irq_reghandler(card->cfg->int_line, &int_handler);

	/* TODO Detect vendor */
	kprintf(KP_INFO, devname, "initialized, %u Hz @ IRQ %u\n",
			io_inw(dev->nambar + 0x2C), card->cfg->int_line);

	return 0;

err:
	kprintf(KP_ERR, devname, "err\n");

	return 1;
}

static void ac97_fini(struct pci_dev *card)
{
	/* TODO */
}

static struct pci_driver driver = {
	.name	= devname,

	.ids	= dev_ids,

	.probe	= &ac97_probe,
	.fini	= &ac97_fini
};

void ac97_init(void)
{
	pci_driver_reg(&driver);
}
