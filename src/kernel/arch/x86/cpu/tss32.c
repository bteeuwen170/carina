/*
 *
 * Elara
 * src/kernel/arch/x86/cpu/tss32.c
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

//#include <print.h>

#include <asm/cpu.h>

//static const char devname[] = "tss";

static struct tss_entry tss[TSS_ENTRIES];

void tss_init(u32 *base, u32 *limit)
{
	/* TODO Fill in the TSS for user mode */

	*base = (u32) &tss;
	*limit = TSS_ENTRIES * sizeof(struct tss_entry);

	//kprintf(KP_INFO, devname,
	//"%d entries entered (FIXME hardcoded lies)\n", IDT_ENTRIES);
}
