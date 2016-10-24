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

//#include <stdint.h> //TEMP
#include <stdlib.h>

#include "ac97.h"
#include "snd.h"

static char *devname = "ac97";

struct pci_dev *card;

u16 nambar, nabmbar;

u16 sample_rate;

u32 buffer_last;
u32 buffer_last_w;

static void *data;

struct buffer {
	void	*buf;
	u16	len;
	u16	reserved :14;
	u8	bup :1;
	u8	ioc :1;
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
	u16 samples = io_inw(nabmbar + 0x18);

	if (!curbuf || samples < 1)
		io_outw(nabmbar + 0x15, 32);

	io_outw(nabmbar + 0x16, (1 << 3));

	buffer_fill(++buffer_last_w, buffer_last);

	buffer_last = curbuf;
}

void ac97_volume(u8 volume)
{
	io_outw(nambar + 0x02, (volume << 8) | volume);
	//io_outw(nambar + 0x04, (volume << 8) | volume);
	io_outw(nambar + 0x06, volume);
	io_outw(nambar + 0x0A, volume);
	io_outw(nambar + 0x18, (volume << 8) | volume);
}

void ac97_play(void)
{
	int i;

	data = snd_wav;

	for (i = 0; i < 32; i++)
		buffer_fill(i, i);

	buffer_last = 0;
	buffer_last_w = 32 - 1;

	//io_outd(nabmbar + 0x10, (intptr_t) buf);
	io_outd(nabmbar + 0x10, (u64) buf);
	io_outb(nabmbar + 0x15, 32);
	io_outb(nabmbar + 0x1B, 0x15);
}

static int pci_handler(struct pci_dev *card)
{
	//kprintf(KP_DBG, devname, "intline: %u\n", card->cfg->int_line);

	/* Set PIO control */
	//TODO
	//pci_outd(card->bus, card->dev, card->func, 4, 5);

	nambar = pci_ind(card->bus, card->dev, card->func, 0x10) & 0xFFFFFFFC;
	nabmbar = pci_ind(card->bus, card->dev, card->func, 0x14) & 0xFFFFFFFC;

	if (!nambar || !nabmbar)
		goto err;

	irq_reghandler(SINT_ENTRIES + card->cfg->int_line, &int_handler);

	/* Reset */
	io_outw(nambar + 0x00, 0x42);
	io_outb(nabmbar + 0x0060, 0x02);

	sleep(100);

	ac97_volume(3);

	sleep(10);

	if ((io_inw(nambar + 0x28) & 1)) {
		kprintf(KP_INFO, devname, ":(\n");
		//ac97_sample_rate(0);
		io_outw(nambar + 0x2A, io_inw(nambar + 0x2A) | 1);
		sleep(10);
		io_outw(nambar + 0x2C, 44100);
		io_outw(nambar + 0x32, 44100);
	} else {
		kprintf(KP_INFO, devname, ":D\n");
	}

	buf = kmalloc(sizeof(struct buffer) * 32);

	kprintf(KP_INFO, devname, "initialized (FIXME lies!)\n");

	return 0;

err:
	kprintf(KP_ERR, devname, "err\n");

	return 1;
}

void ac97_reghandler(void)
{
	pci_reghandler(0x8086, 0x2415, 1, &pci_handler);
}
