/*
 *
 * Elara
 * src/kernel/arch/x86_64/cpu/gdt32.c
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

//#include <kernel.h>

#include <asm/cpu.h>

//static const char devname[] = "gdt";

static struct segment_desc gdt[GDT_ENTRIES - 1];

static void gdt_set(const u8 gate, const u32 base, const u32 limit,
		const u8 type, const u8 lm)
{
	gdt[gate].limit_lo	= limit & 0xFFFF;
	gdt[gate].base_lo[0]	= base & 0xFF;
	gdt[gate].base_lo[1]	= (base >> 8) & 0xFF;
	gdt[gate].base_lo[2]	= (base >> 16) & 0xFF;
	gdt[gate].flags		= (lm << 13) | type | 0b10000000 |
			(((limit >> 16) & 0xF) << 8);
	gdt[gate].base_hi	= (base >> 24) & 0xFF;
}

/* FIXME Not i686 compatible ! */
void gdt_init(void)
{
	u32 tss_base, tss_limit;

	/* 0x08 - Code SV */
	gdt_set(1, 0, 0, 0b0011000, 1);
	/* 0x10 - Data SV */
	gdt_set(2, 0, 0, 0b0010000, 0);
	/* 0x18 - Code USR */
	gdt_set(3, 0, 0, 0b1111000, 1);
	/* 0x20 - Data USR */
	gdt_set(4, 0, 0, 0b1110010, 0);

	tss_init(&tss_base, &tss_limit);

	/* 0x28 - TSS */
	gdt_set(5, tss_base, tss_limit, 0b0001001, 1);

	gdt_load(&gdt, GDT_ENTRIES * sizeof(struct segment_desc) - 1);
	gdt_load(&gdt, 5 * sizeof(struct segment_desc) - 1);

	//tss_load(); //FIXME

	//kprintf32(KP_INFO, devname,
	//"%d entries entered (FIXME hardcoded lies)\n", IDT_ENTRIES);
}
