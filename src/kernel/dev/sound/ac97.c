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

static char *devname = "ac97";

struct ac97_dev {
	u16 sample_rate;

	u32	buffer_last;
	u32	buffer_last_w;
};

struct pci_dev *card;

u16 nambar, nabmbar;

u16 sample_rate;

u32 buffer_last;
u32 buffer_last_w;

struct buffer {
	u32	buf;
	u16	len;
	u16	reserved :14;
	u8	bup :1;
	u8	ioc :1;
} __attribute__ ((packed));

struct buffer *buf;

static void buffer_fill(void * data, u32 off, u32 n)
{
	buf[n].buf = (u32) (u64) (&data + (32 * 2 * off));
	buf[n].len = 32;
	buf[n].bup = 0;
	buf[n].ioc = 1;
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

	buffer_fill(snd_wav, ++buffer_last_w, buffer_last);

	buffer_last = curbuf;
}

static void ac97_volume(u8 volume)
{
	io_outw(nambar + 0x02, (volume << 8) | volume);
	//io_outw(nambar + 0x04, (volume << 8) | volume);
	io_outw(nambar + 0x06, volume);
	io_outw(nambar + 0x0A, volume);
	io_outw(nambar + 0x18, (volume << 8) | volume);
	sleep(10);
}

void ac97_play(void)
{
	int i;

	for (i = 0; i < 32; i++)
		buffer_fill(snd_wav, i, i);

	buffer_last = 0;
	buffer_last_w = 32 - 1;

	io_outb(nabmbar + 0x15, buffer_last);
	/* io_outb(nabmbar + 0x1B, 0x19); */
	io_outb(nabmbar + 0x1B, io_inb(nabmbar + 0x1B) | 0x01);

	kprintf(KP_INFO, devname, "wav playing\n");
}

static int pci_handler(struct pci_dev *card)
{
	kprintf(KP_DBG, devname, "intline: %u\n", card->cfg->int_line);

	nambar = card->cfg->bar_0 - 1;
	nabmbar = card->cfg->bar_1 - 1;

	if (!nambar || !nabmbar)
		goto err;

	/* Set PIO control */
	pci_outd(card->bus, card->dev, card->func, 4, 5);

	/* Reset */
	io_outw(nambar + 0x00, 0x42);
	io_outb(nabmbar + 0x0B, 0x02);
	io_outb(nabmbar + 0x1B, 0x02);
	io_outb(nabmbar + 0x2B, 0x02);
	sleep(100);

	if (io_inw(nambar + 0x28) & 1) {
		/* ac97_sample_rate(0); */
		io_outw(nambar + 0x2A, io_inw(nambar + 0x2A) | 0x01);
		sleep(10);
		io_outw(nambar + 0x2C, 48000);
		io_outw(nambar + 0x32, 48000);
	}

	kprintf(KP_DBG, devname, "sr: %u Hz\n", io_inw(nambar + 0x2C));

	ac97_volume(0);

	buf = kmalloc(sizeof(struct buffer) * 32);
	io_outd(nabmbar + 0x10, (intptr_t) buf);

	irq_reghandler(card->cfg->int_line, &int_handler);

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
