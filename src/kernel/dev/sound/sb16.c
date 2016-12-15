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

//#include <stdint.h> /* TEMP */
#include <stdlib.h>
#include <string.h> /* TEMP */

#include "snd.h"

static char *devname = "sb16";

void sb16_play(void)
{

	kprintf(KP_INFO, devname, "wav playing\n");
}

int sb16_init(void)
{
	return 0;

/* err:
	kprintf(KP_ERR, devname, "err\n");

	return 1; */
}
