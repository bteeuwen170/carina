/*
 *
 * Elarix
 * src/kernel/dev/sound/es1370.c
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

#include <delay.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <kernel.h>
#include <mm.h>
#include <module.h>
#include <pci.h>

#include <asm/cpu.h>

static const char devname[] = "es1370";

#define ES1370_REG_CTRL		0x00
#define ES1370_REG_STATUS	0x04
#define ES1370_REG_MEMPAGE	0x0C
#define ES1370_REG_CODEC	0x10
#define ES1370_REG_LEGACY	0x18
#define ES1370_REG_SERIAL	0x20

#define ES1370_BUF_ADDR		0x38
#define ES1370_BUF_SIZE		0x3C
#define ES1370_BUF_LENGTH	0x28

#define ES1370_CMD_RESET	0x20
#define ES1370_CMD_CODEC_RESET	0x1603

/* struct bdl {
	u32	addr;
	u16	len;
	u16	reserved:14;
	u8	bup:1;
	u8	ioc:1;
} __attribute__ ((packed)); */

struct es1370_dev {
	u16	addr;
	char	*buf;
};

/* TEMP */
static struct es1370_dev deva;
static struct es1370_dev *dev = &deva;

char *audio;

static int int_handler(struct int_stack *regs)
{
	return 1;
}

static void volume_set(void)
{
	int i;

	io_outw(dev->addr + ES1370_REG_CODEC, 0x1800);
		sleep(10);

	for (i = 0; i <= 0x19; i++) {
		io_outw(dev->addr + ES1370_REG_CODEC, (i << 8) | 0x08);
		sleep(10);
	}
}

void es1370_play(void)
{
	int i;

	struct file *fp = NULL;
	file_open("/snd.pcm", F_RO, &fp);
	if (!fp)
		dprintf("CRAP OPEN\n");
	audio = kmalloc(fp->ip->size);
	if (!file_read(fp, audio, fp->ip->size))
		dprintf("CRAP READ\n");

	for (i = 0; i < 60; i++)
		dev->buf[i] = audio + i * 60;

	io_outd(dev->addr + ES1370_BUF_ADDR, (uintptr_t) dev->buf);
	io_outd(dev->addr + ES1370_BUF_SIZE, 8 - 1);
	io_outd(dev->addr + ES1370_BUF_LENGTH, 2000);

	/* TODO */
	io_outd(dev->addr + ES1370_REG_SERIAL, 0x0020020C);

	io_outw(dev->addr + ES1370_REG_CTRL, io_inw(dev->addr + ES1370_REG_CTRL) | 0x00000020);

	dprintf(KP_DBG "playing\n");
}

static int es1370_probe(struct device *devp)
{
	struct pci_cfg *cfgp;
	int res, i, j;

	cfgp = devp->bus;

	/* Set SERR#, BME and IOSE */
	/* FIXME pci_outd(cfgp->bus, cfgp->dev, cfp->func, 0x04, 0x05); */
	pci_outd(0, 4, 0, 0x04, 0b00100000101);

	if (!(dev->addr = cfgp->bar_0 & ~1))
		return -EBUSY;

	/* Reset */
	io_outd(dev->addr + ES1370_REG_CTRL, 0);
	io_outd(dev->addr + ES1370_REG_LEGACY, 0);
	io_outd(dev->addr + ES1370_REG_SERIAL, 0);
	sleep(20);

	/* Set XCTL0 and CDC_EN */
	io_outw(dev->addr + ES1370_REG_CTRL, 0b100000010);

	/* Reset codec */
	io_outw(dev->addr + ES1370_REG_CODEC, ES1370_CMD_CODEC_RESET);
	sleep(20);
	io_outw(dev->addr + ES1370_REG_CODEC, 0x1700);
	sleep(20);

	volume_set();

	io_outb(dev->addr + ES1370_REG_MEMPAGE, ES1370_REG_MEMPAGE);

	if (!(dev->buf = kcalloc(59, sizeof(char)))) {
		res = -ENOMEM;
		goto err;
	}

	if ((res = irq_handler_reg(cfgp->int_line, &int_handler)) < 0)
		return res;

	/* TODO Detect vendor and assign name */
	/* FIXME Use new device minor instead of controller minor */
	dprintf(KP_CON "initialized! @ IRQ %u\n", cfgp->int_line);
	/* dprintf(KP_CON "%u: %u Hz @ IRQ %u\n", MINOR(devp->dev),
			io_inw(dev->nambar + AC97_NAMBAR_RATE_PO_FRONT),
			cfgp->int_line); */

	return 0;

err:
	irq_handler_unreg(cfgp->int_line);

	return res;
}

static void es1370_fini(struct device *devp)
{
	(void) devp;

	/* TODO */
}

static const struct pci_id es1370_pci_ids[] = {
	/* ICH */
	PCI_ID(0x1274, PCI_ID_ANY, 0x5000, PCI_ID_ANY, 0x04, 0x01, 0)
};

static struct driver es1370_driver = {
	.name	= devname,
	.major	= MAJOR_SND,

	.busid	= BUS_PCI,
	.bus	= &es1370_pci_ids,

	.probe	= &es1370_probe,
	.fini	= &es1370_fini
};

int es1370_init(void)
{
	return driver_reg(&es1370_driver);
}

void es1370_exit(void)
{
	driver_unreg(&es1370_driver);
}

MODULE(es1370, &es1370_init, &es1370_exit);
MODULE_DEP(pci);
