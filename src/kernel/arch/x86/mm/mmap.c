/*
 *
 * Elarix
 * src/kernel/arch/x86/mm/mmap.c
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

#include <kernel.h>
#include <mboot.h>

#include <asm/cpu.h>

static const char devname[] = "mmap";

static struct mboot_mmap *mmap;
static size_t mmap_len, mem_max;

uintptr_t mmap_get(uintptr_t page)
{
	struct mboot_mmap *cmmap;
	uintptr_t i, pi = 0;

	if (page > mem_max / PAGE_SIZE)
		return NULL;

	cmmap = mmap;

	do {
		if (cmmap->type != 1)
			continue;

		for (i = 0; i < cmmap->len; i += PAGE_SIZE)
			if (pi++ == page)
				return (void *) (cmmap->addr + i);
	} while (++cmmap < mmap + mmap_len);

	return NULL;
}

void mmap_init(uintptr_t addr, size_t len)
{
	dprintf("Physical memory map\n");

	mmap = (struct mboot_mmap *) addr;
	do {
		dprintf(KP_CON "%#018lx - %#018lx", mmap->addr,
				mmap->addr + mmap->len);

		if (mmap->type == 1) {
			kprintf(" (free)\n");
			mem_max += mmap->len;
		} else if (mmap->type == 3) {
			kprintf(" (acpi)\n");
		} else {
			kprintf("\n");
		}
	} while ((uintptr_t) ++mmap < addr + len);

	mmap = (struct mboot_mmap *) addr;
	mmap_len = len;

	dprintf("%u MB memory\n", mem_max / 1024 / 1024 + 1);
}
