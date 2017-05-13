/*
 *
 * Elarix
 * src/kernel/dev/sound/ac97.c
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

/* FIXME Interrupts are not working */

#include <errno.h>
#include <fs.h>
#include <dev.h>
#include <delay.h>
#include <kernel.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

#include <stdlib.h>

static const char devname[] = "ac97";

#define AC97_NAMBAR_RESET	0x00
#define AC97_NAMBAR_POWER	0x26

#define AC97_NAMBAR_VOL_MASTER	0x02
#define AC97_NAMBAR_VOL_PO	0x18

#define AC97_NAMBAR_AUDIO_EXT		0x28
#define AC97_NAMBAR_AUDIO_EXT_CR	0x2A

#define AC97_NAMBAR_SRATE_PO_FRONT	0x2C
#define AC97_NAMBAR_SRATE_PO_SURROUND	0x2E
#define AC97_NAMBAR_SRATE_PO_LFE	0x30
#define AC97_NAMBAR_SRATE_PO_LR		0x32

#define AC97_NABMBAR_PO_BDBAR	0x10
#define AC97_NABMBAR_GLOB_STA	0x30

#define AC97_NABMBAR_PO_LVI	0x15
#define AC97_NABMBAR_PO_SR	0x16
#define AC97_NABMBAR_PO_CR	0x1B

struct bdl {
	u32	addr;
	u16	len;
	u16	reserved:14;
	u8	bup:1;
	u8	ioc:1;
} __attribute__ ((packed));

struct ac97_dev {
	u16	nambar;	/* Native Audio Mixer base address */
	u16	nabmbar;	/* Native Audio Bus Mastering base address */
	/* u16	sample_rate; */

	struct bdl *buf;
	u32	index;
	u32	prev;
};

/* TEMP */
static struct ac97_dev deva;
static struct ac97_dev *dev = &deva;

char *audio;

static void bdl_fill(char *data, u32 n, u32 off)
{
	/* TODO Physical address */
	dev->buf[n].addr = (uintptr_t) data + off * 32 * 2;
	dev->buf[n].len = 32;
	dev->buf[n].bup = 0;
	dev->buf[n].ioc = 1;
}

static int int_handler(struct int_stack *regs)
{
	u16 status;
	u32 buf;
	(void) regs;

	/* kprintf("!"); */

	status = io_inw(dev->nabmbar + AC97_NABMBAR_PO_SR);

	if (status & 0b1000) {
		buf = (dev->index + 1) % 32;
		bdl_fill(audio, dev->prev, ++dev->index);
		dev->prev = buf;

		io_outb(dev->nabmbar + AC97_NABMBAR_PO_LVI, 32);
		io_outw(dev->nabmbar + AC97_NABMBAR_PO_SR, 0b1000);

		/* dprintf(KP_DBG "sc: %u\n",
				io_inw(dev->nabmbar + AC97_NABMBAR_PO_SR)); */
	}

	return 1;
}

static void volume_set(u8 volume)
{
	io_outw(dev->nambar + AC97_NAMBAR_VOL_MASTER, (volume << 8) | volume);
	io_outw(dev->nambar + AC97_NAMBAR_VOL_PO, (volume << 8) | volume);

	sleep(10);
}

#include "snd.h"

void ac97_play(void)
{
	int i;

#if 1
	audio = snd_wav - 0x80000000;
#else
	struct file *fp = NULL;
	file_open("/snd.pcm", F_RO, &fp);
	if (!fp)
		dprintf("CRAP OPEN\n");
	audio = kmalloc(fp->ip->size);
	if (!file_read(fp, audio, fp->ip->size))
		dprintf("CRAP READ\n");
#endif

	for (i = 0; i < 32; i++)
		bdl_fill(audio, i, i);

	dev->index = 32 - 1;
	dev->prev = 0;

	io_outb(dev->nabmbar + AC97_NABMBAR_PO_LVI, 32);
	/* Set IOCE, FEIE and RPBM */
	io_outb(dev->nabmbar + AC97_NABMBAR_PO_CR, 0b11001);

	dprintf(KP_DBG "playing @ %#x\n", audio);
	kprintf("status: %d\n", io_inw(dev->nabmbar + AC97_NABMBAR_PO_SR));
}

static int ac97_probe(struct device *devp)
{
	struct pci_cfg *cfgp;
	int res;

	cfgp = devp->bus;

	/* TODO Check codec ID before assuming it is 0 */
	dev->nambar = cfgp->bar_0 & ~1;
	dev->nabmbar = cfgp->bar_1 & ~1;

	if (!dev->nambar || !dev->nabmbar) {
		res = -EBUSY;
		goto err;
	}

	/* Reset */
	io_outw(dev->nambar + AC97_NAMBAR_POWER, 0);
	io_outw(dev->nambar + AC97_NAMBAR_RESET, 0);
	sleep(20);

	/* Check if codec is ready */
	/* if (!(io_ind(dev->nabmbar + AC97_NABMBAR_GLOB_STA) & 0b100000000)) {
		res = -EIO;
		goto err;
	} */

	/* XXX Not sure if right; Check if device is ready */
	/* if (!(io_inw(dev->nambar + AC97_NAMBAR_POWER) & 0x03))
		return -EIO; */

	/* Set BME and IOSE */
	pci_outd(devp->busid, 0x04, 0b00000000101);

	if ((res = irq_handler_reg(cfgp->int_line, &int_handler)) < 0)
		goto err;

	/* Register Reset */
	/* io_outb(dev->nabmbar + AC97_NABMBAR_PO_CR, 0b10); */
	/* sleep(10); */

	/* Set IOCE and FEIE */
	io_outb(dev->nabmbar + AC97_NABMBAR_PO_CR, 0b11000);

#if 1

	/* Check if sample rate is set out of the box */
	if (io_inw(dev->nambar + AC97_NAMBAR_AUDIO_EXT) & 1) {
		/* XXX Right? */
		io_outw(dev->nambar + AC97_NAMBAR_AUDIO_EXT_CR, 1);
		sleep(10);

		io_outw(dev->nambar + AC97_NAMBAR_SRATE_PO_FRONT, 48000);
		io_outw(dev->nambar + AC97_NAMBAR_SRATE_PO_SURROUND, 48000);
		io_outw(dev->nambar + AC97_NAMBAR_SRATE_PO_LFE, 48000);
		io_outw(dev->nambar + AC97_NAMBAR_SRATE_PO_LR, 48000);
	}
#endif

	volume_set(1);

	if (!(dev->buf = kcalloc(32, sizeof(struct bdl)))) {
		res = -ENOMEM;
		goto err;
	}

	/* TODO Physical address */
	io_outd(dev->nabmbar + AC97_NABMBAR_PO_BDBAR, (uintptr_t) dev->buf);

	/* TODO Detect vendor and assign name */
	/* FIXME Use new device minor instead of controller minor */
	dprintf(KP_CON "%u: %u Hz @ IRQ %u\n", MINOR(devp->dev),
			io_inw(dev->nambar + AC97_NAMBAR_SRATE_PO_FRONT),
			cfgp->int_line);

	return 0;

err:
	irq_handler_unreg(cfgp->int_line);

	return res;
}

static void ac97_fini(struct device *devp)
{
	(void) devp;

	/* TODO */
}

static const struct pci_id ac97_pci_ids[] = {
	/* ICH */
	PCI_ID(0x8086, PCI_ID_ANY, 0x2415, PCI_ID_ANY, 0x04, 0x01, 0),
	/* ICH0 */
	PCI_ID(0x8086, PCI_ID_ANY, 0x2425, PCI_ID_ANY, 0x04, 0x01, 0),
	/* ICH2 */
	PCI_ID(0x8086, PCI_ID_ANY, 0x2445, PCI_ID_ANY, 0x04, 0x01, 0),
	/* ICH4 */
	PCI_ID(0x8086, PCI_ID_ANY, 0x24C5, PCI_ID_ANY, 0x04, 0x01, 0)
};

static struct driver ac97_driver = {
	.name	= devname,
	.major	= MAJOR_SND,

	.busid	= BUS_PCI,
	.bus	= &ac97_pci_ids,

	.probe	= &ac97_probe,
	.fini	= &ac97_fini
};

int ac97_init(void)
{
	return driver_reg(&ac97_driver);
}

void ac97_exit(void)
{
	driver_unreg(&ac97_driver);
}

MODULE(ac97, &ac97_init, &ac97_exit);
MODULE_DEP(pci);
