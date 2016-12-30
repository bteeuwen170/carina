/*
 *
 * Elarix
 * src/kernel/dev/sound/sb16.c
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

#include <fs.h>
#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

#include <timer/pit.h>

#include <stdlib.h>
#include <string.h> /* TEMP */

/* #include "snd.h" */

static const char devname[] = "sb16";

static int int_handler(struct int_stack *regs)
{
	(void) regs;

	dprintf(devname, "!\n");

	return 1;
}

void sb16_play(void)
{
	dprintf(devname, "wav playing\n");
}

static int sb16_probe(struct device *card)
{
	int res;
	(void) card;

	res = irq_handler_reg(10, &int_handler);

	if (res < 0)
		goto err;

	io_outb(0x220 + 0x06, 0x01);
	io_outb(0x220 + 0x06, 0x00);

	if (!(io_inb(0x220 + 0x0E) & 0x80))
		return 1;

	if (io_inb(0x220 + 0x0A) != 0xAA)
		return 1;

	sleep(20);

	if ((io_inb(0x220 + 0x0C) & 0x80) != 0)
		goto err;

	io_outb(0x220 + 0x0C, 0xE1);

	io_outb(0x220 + 0x04, 0x80);
	io_outb(0x220 + 0x05, (1 << 2));

	dprintf(devname, "initialized, %u Hz @ IRQ %u\n",
			0, 10);

	dprintf(devname, KP_DBG "DSP %d.%d\n",
			io_inb(0x220 + 0x0A), io_inb(0x220 + 0x0A));

	return res;

err:
	irq_handler_unreg(10);

	dprintf(devname, KP_ERR "unable to intialize sb16 card\n");

	return res;
}

/* static void sb16_fini(struct device *card)
{
	(void) card;
} */

int sb16_init(void)
{
	return sb16_probe(NULL);
}

void sb16_exit(void)
{
}

MODULE(sb16, &sb16_init, &sb16_exit);
