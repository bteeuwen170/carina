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

#include <errno.h>
#include <kernel.h>
#include <mboot.h>
#include <mm.h>

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

static void *start, *end;
static uintptr_t max;

void *kmalloc(size_t size, int flags)
{
	struct frame *cap, *nap;
	size_t i;
	int free = 0;

	if (!size)
		return NULL;

	nap = end;

	if (end + 2 * sizeof(struct frame) + nap->size + size < max)
		free = 1;

	if (flags & KM_CONT) {
		nap->next = block_alloc_kernel(size);
		end = nap->next;
		max = end + size;

		kprintf("done!, max = %#lx\n", max);

		goto new;
	}

	for (cap = start; cap; cap = cap->next) {
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

		/*
		 * TODO
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

#ifdef CONFIG_ALLOC_VERBOSE
		kprintf("& @ %#x (%u), %#x (%u) < %u\n", cap->addr, size,
				nap->addr, nap->size, cap->size);
#endif

		cap->size = size;
		cap->next = nap;

		goto ret;
	}

	if (!free) {
		for (i = 0; i < size / PAGE_SIZE + ((size % PAGE_SIZE) ? 1 : 0);
				i++) {
			if (!(page_alloc_kernel()))
				return NULL;
			max += PAGE_SIZE;
		}
	}

new:
	if (!(flags & KM_CONT) && nap->size) {
		end += sizeof(struct frame) + nap->size;
		nap->next = end;
	}

	cap = end;
	cap->addr = (uintptr_t) cap + sizeof(struct frame);
	cap->size = size;
	cap->next = NULL;

#ifdef CONFIG_ALLOC_VERBOSE
	kprintf("+ @ %#x (%u)\n", cap->addr, cap->size);
#endif

ret:
	cap->present = 1;

	return (void *) cap->addr;
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

	for (cap = start; cap; cap = cap->next) {
		if (cap->addr == (uintptr_t) ptr) {
			if (cap->present)
				goto found;
			else
				break;
		}
	}

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

struct mem_info *mm_mem_info(void)
{
	struct mem_info *mip;
	struct frame *cap;

	if (!(mip = kmalloc(sizeof(struct mem_info), 0)))
		return NULL;

	mip->total = mmap_mem_max();

	cap = end;
	mip->allocated = max - (uintptr_t) start;

	mip->free = mip->total - mip->allocated;
	mip->used = 0;

	for (cap = start; cap; cap = cap->next)
		if (cap->present)
			mip->used += cap->size;

	return mip;
}

void mm_init(void)
{
	if (!(start = end = page_alloc_kernel()))
		panic("unable to allocate first page", -ENOMEM, 0);
	max = end + PAGE_SIZE;

	memset(start, 0, sizeof(struct frame));
}
