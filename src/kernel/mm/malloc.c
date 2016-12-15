/*
 *
 * Elara
 * src/kernel/mm/malloc.c
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

#include <mboot.h>
#include <print.h>

static char *devname = "mem";

const char *mmap_types[5] = {
	"rsvd",
	"free",
	"rsvd",
	"ACPI",
	"rsvd"
};

/* FIXME This is utter crap */
#define BLOCK_SIZE	4096
#define BLOCKS_MAX	(BLOCK_SIZE * sizeof(u8))

extern u64 kern_end;

static intptr_t position;

struct mem_block {
	u8			allocated;
	struct mem_block	*prev;
	struct mem_block	*next;
} *blocks;

void *kmalloc(u64 size)
{
	if (size == 0)
		return NULL;

	void *p = (void *) position;

	position += size;

	return p;
}

/*void *krealloc(void *ptr, u64 size)
{
	void *p = kmalloc(size);

	if (!p)
		return NULL;

	//memcpy(p, ptr,

	kfree(ptr);

	return p;
}*/

void kfree(void *ptr)
{
	(void) ptr;
	return;
}

void mm_init(u32 addr, u32 len)
{
	struct mboot_mmap *mmap = (void *) (intptr_t) addr;
	u64 mem = 0;

	kprintf(KP_INFO, devname, "Physical memory map:\n");

	while ((u64) mmap < addr + len) {
		u64 maddr = mmap->addr_lo | (mmap->addr_hi >> 16);
		u64 mlen = mmap->len_lo | (mmap->len_hi >> 16);

		kprintf(KP_INFO | KP_CON, devname,
				"%#018lx - %#018lx (%s)\n",
				maddr, maddr + mlen, mmap_types[mmap->type]);

		mem += mlen;

		mmap = (struct mboot_mmap *)
				((u64) mmap + mmap->size + sizeof(mmap->size));
	}

	/* FIXME Always off by 1 for some reason */
	kprintf(KP_INFO, devname, "%u MB memory\n", mem / 1024 / 1024 + 1);

	/* FIXME How much padding is really required? */
	//position = ((intptr_t) &kern_end) + 0x10000;
	position = 0x200000;

	/*u16 i;

	for (i = 0; i < BLOCKS_MAX; i++) {
		blocks->allocated = 0;
	}*/
}
