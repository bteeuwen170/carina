/*
 *
 * Carina
 * Local APIC Handler
 *
 * Copyright (C) 2015 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>

bool lapic_avail(void) {
	u32 eax, edx;
	cpuid(1, &eax, NULL, NULL, &edx);

	return edx & 0x200;
}

i8 lapic_init(void)
{
	//TODO Check if LAPICs are present

	u64 flags = msr_in(0x1B) & 0x0F00;
	flags |= (u64) lapic_avail() << 10;
	flags |= 1 << 11;
	msr_out(0x1B, 0xFEE00000 | flags);

	return OK;
}

//static i8 lapic_timer_calibrate(u64 

i8 lapic_timer_init(void *master)
{
	asm volatile ("sti");

	asm volatile ("cli");
}
