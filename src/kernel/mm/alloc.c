/*
 *
 * Elarix
 * src/kernel/mm/alloc.c
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

#include <mboot.h>
#include <kernel.h>

#include <asm/cpu.h>

#include <string.h>

static const char devname[] = "mm";

/* Minimum allocation size to always allocate at end of heap (0 to disable */
#define NEW_MIN		4096
/* Minimum size for split frames (+ sizeof(struct frame)) */
#define SIZE_MIN	8

struct frame {
	uintptr_t	addr; /* XXX Don't really need this */
	size_t		size;
	char		present:1;

	struct frame	*next;
};
/* FIXME Option for page aligned */
/* } __attribute__ ((packed)); */

extern uintptr_t kern_end;
static void *start, *end, *max;

void *kmalloc(size_t size)
{
	struct frame *cap, *nap, *oap;
	off_t poff;
	int free = 0;

	if (!size)
		return NULL;

	cap = start;
	nap = end;

	if (end + 2 * sizeof(struct frame) + nap->size + size - VM_ADDR < max)
		free = 1;

	do {
		if (cap->present || cap->size < size)
			continue;

		if (cap->size == size) {
#ifdef CONFIG_ALLOC_VERBOSE
			kprintf("* @ %#x (%u)\n", cap->addr, cap->size);
#endif
			goto ret;
		} else if (free && size >= NEW_MIN) {
			goto new;
		}

		if (free && cap->size - size < sizeof(struct frame) + SIZE_MIN)
			continue;

		/* TODO
		 * - Combine frames *
		 * - Don't take first fit (to split),
		 *   search certain offset for perfect fit.
		 * - Also don't split if frame is significantly bigger
		 */

		nap = (struct frame *) (cap->addr + size);
		nap->addr = (uintptr_t) nap + sizeof(struct frame);
		nap->size = cap->size - sizeof(struct frame) - size;
		nap->present = 0;
		nap->next = cap->next;

		cap->size = size;
		cap->next = nap;

#ifdef CONFIG_ALLOC_VERBOSE
		kprintf("& @ %#x (%u)\n", cap->addr, cap->size);
#endif

		goto ret;
	} while ((cap = cap->next));

	/* TODO Allocate new page */
	if (!free)
		return NULL;

new:
	if (nap->size) {
		end += sizeof(struct frame) + nap->size;
		nap->next = end;
	}

	cap = end;
	cap->addr = (uintptr_t) end + sizeof(struct frame);
	cap->size = size;
	cap->next = NULL;

#ifdef CONFIG_ALLOC_VERBOSE
	kprintf("+ @ %#x (%u)\n", cap->addr, cap->size);
#endif

ret:
	cap->present = 1;

	return (void *) cap->addr;
}

/* An extemely crappy implementation of calloc */
void *kcalloc(size_t nmemb, size_t size)
{
	void *p = kmalloc(nmemb * size);

	memset(p, 0, nmemb * size);

	return p;
}

void kfree(void *ptr)
{
	struct frame *cap;

	if (!ptr) {
#ifdef CONFIG_ALLOC_VERBOSE
	kprintf("- @ NULL\n");
#endif
		return;
	}

	cap = start;

	do {
		if (cap->addr == (uintptr_t) ptr) {
			if (cap->present)
				goto found;
			else
				break;
		}
	} while ((cap = cap->next));

	panic("attempted to free unallocated memory", 0, (uintptr_t) ptr);

found:
#ifdef CONFIG_ALLOC_FREE_ZERO
	memset((void *) cap->addr, 0, cap->size);
#endif
	cap->present = 0;

#ifdef CONFIG_ALLOC_VERBOSE
	kprintf("- @ %#x (%u)\n", cap->addr, cap->size);
#endif

	return;
}

/* XXX How arch. specific is this? */
void mm_init(uintptr_t addr, size_t len)
{
	struct mboot_mmap *mmap = (void *) addr;

	dprintf("Physical memory map:\n");

	while ((uintptr_t) mmap < addr + len) {
		uintptr_t maddr = mmap->addr_lo | (mmap->addr_hi >> 16);
		uintptr_t mlen = mmap->len_lo | (mmap->len_hi >> 16);

		dprintf(KP_CON "%#018lx - %#018lx",
				maddr, maddr + mlen);
		if (mmap->type == 1)
			kprintf(" (free)");
		else if (mmap->type == 3)
			kprintf(" (acpi)");
		kprintf("\n");

		max += mlen;

		mmap = (struct mboot_mmap *)
				((uintptr_t)
				 mmap + mmap->size + sizeof(mmap->size));
	}

	dprintf("%u MB memory\n", (uintptr_t) max / 1024 / 1024 + 1);

	/* FIXME Get proper end of kernel */
	start = end = (void *) ((uintptr_t) &kern_end + 0x30000);
	((struct frame *) start)->size = 0;
}
