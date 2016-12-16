/*
 *
 * Elara
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

#include <print.h>

#include <asm/cpu.h>

#include <timer/pit.h>

#include <stdlib.h>
#include <string.h> /* TEMP */

#include "snd.h"

static char *devname = "sb16";

static void int_handler(struct int_stack *regs)
{
	kprintf(KP_INFO, devname, "!\n");
}

void sb16_play(void)
{

	kprintf(KP_INFO, devname, "wav playing\n");
}

int sb16_init(void)
{
	int i;

	/* Detect DSP version */

	io_outb(0x220 + 0x06, 0x01);
	io_outb(0x220 + 0x06, 0x00);

	for (i = 0; i < 10; i++)
		if (io_inb(0x220 + 0x0E) & 0x80)
			break;

	if (i == 10)
		goto err;

	for (i = 0; i < 10; i++)
		if (io_inb(0x220 + 0x0A) == 0xAA)
			break;

	if (i == 10)
		goto err;

	for (i = 0; i < 100; i++) {
		if (io_inb(0x220 + 0x0C) & 0x80) {
			sleep(100);
			continue;
		}

		io_outb(0x220 + 0x0C, 0xE1);
		break;
	}

	kprintf(KP_DBG, devname, "DSP %d.%d\n",
			io_inb(0x220 + 0x0A), io_inb(0x220 + 0x0A));

	/* Set IRQ */

	io_outb(0x220 + 0x04, 0x80);
	io_outb(0x220 + 0x05, (1 << 2));

	irq_reghandler(7, &int_handler);

	return 0;

err:
	kprintf(KP_ERR, devname, "err\n");

	return 1;
}
