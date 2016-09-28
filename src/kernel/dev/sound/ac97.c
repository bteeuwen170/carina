/*
 *
 * Carina
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

#include <stdint.h> //TEMP
#include <stdlib.h>
#include <system.h>
#include <cpu.h>
#include <kernel/print.h>
#include <pci/pci.h>
#include "ac97.h"
//#include "snd.h"

static char *devname = "ac97";

struct pci_dev *card;

u16 bar_nam, bar_nabm;

u16 sample_rate;

u32 buffer_last;
u32 buffer_last_w;

static void *data;

struct buffer {
	void	*buf;
	u16		len;
	u16		reserve	:14;
	u8		bup		:1;
	u8		ioc		:1;
} __attribute__ ((packed));

struct buffer *buf;

static void buffer_fill(u32 off, u32 n)
{
	buf[n].buf = (void *) (&data + (32 * 2 * off));
	buf[n].len = 32;
	buf[n].ioc = 1;
	buf[n].bup = 0;
}

static void int_handler(struct int_stack *regs)
{
	(void) regs;

	kprintf(KP_DBG, devname, "!"); //TEMPORARY
	u32 curbuf = (buffer_last + 1) % 32;
	u16 samples = io_ins(bar_nabm + 0x18);

	if (!curbuf || samples < 1)
		io_outs(bar_nabm + 0x15, 32);

	io_outs(bar_nabm + 0x16, (1 << 3));

	buffer_fill(++buffer_last_w, buffer_last);

	buffer_last = curbuf;
}

void ac97_volume(u8 volume)
{
	io_outs(bar_nam + 0x02, (volume << 8) | volume);
	//io_outs(bar_nam + 0x04, (volume << 8) | volume);
	//io_outs(bar_nam + 0x06, volume);
	//io_outs(bar_nam + 0x0A, volume);
	io_outs(bar_nam + 0x18, (volume << 8) | volume);
}

void ac97_play(void)
{
	int i;

	//data = snd_wav;

	for (i = 0; i < 32; i++)
		buffer_fill(i, i);

	buffer_last = 0;
	buffer_last_w = 32 - 1;

	io_outc(bar_nabm + 0x15, 32);
	io_outc(bar_nabm + 0x1B, (1 << 0) | (1 << 3) | (1 << 4));
}

static int pci_handler(struct pci_dev *card)
{
	//kprintf(KP_DBG, devname, "intline: %u\n", card->cfg->int_line);

	/* Set PIO control */
	//TODO
	pci_outi(card->bus, card->dev, card->func, 4, 5);

	bar_nam = pci_ini(card->bus, card->dev, card->func, 0x10) & 0xFFFFFFFC;
	bar_nabm = pci_ini(card->bus, card->dev, card->func, 0x14) & 0xFFFFFFFC;

	if (!bar_nam || !bar_nabm)
		goto err;

	irq_reghandler(SINT_ENTRIES + card->cfg->int_line, &int_handler);

	ac97_volume(3);

	//ac97_sample_rate(0);

	buf = kmalloc(sizeof(struct buffer) * 32);
	io_outi(bar_nabm + 0x10, (intptr_t) buf);

	kprintf(KP_INFO, devname, "initialized (FIXME lies!)\n");

	return 0;

err:
	kprintf(0, devname, "err\n");

	return 1;
}

void ac97_reghandler(void)
{
	pci_reghandler(0x8086, 0x2415, 1, &pci_handler);
}
