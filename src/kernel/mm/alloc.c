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

#include <string.h>

static const char devname[] = "mm";

#if 0
void *kmalloc(size_t size)
{

}

void *kcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = kmalloc(nmemb * size)))
		return p;

	memset(p, 0, nmemb * size);

	return p;
}

void kfree(void *addr)
{

}
#else
static const char *mmap_types[5] = {
	"rsvd",
	"free",
	"rsvd",
	"ACPI",
	"rsvd"
};

static uintptr_t position;

struct addr {
	uintptr_t	addr;
	size_t		size;
	char		present:1;

	struct addr	*next;
};

static struct addr addresses;
static struct addr *last_addr = &addresses;

void *kmalloc(size_t size)
{
#if 1
	if (!size)
		return NULL;

	last_addr->next = (struct addr *) position;
	last_addr = (struct addr *) position;
	position += sizeof(struct addr);

	last_addr->addr = position;
	last_addr->size = size;
	last_addr->present = 1;

	position += size;

	/* kprintf("+"); */

	return (void *) last_addr->addr;
#else
	void *p;

	if (size == 0)
		return NULL;

	p = (void *) position;

	position += size;

	kprintf("+");

	return p;
#endif
}

/* XXX an extemely crappy implementation of calloc */
void *kcalloc(size_t nmemb, size_t size)
{
	void *p = kmalloc(nmemb * size);

	memset(p, 0, nmemb * size);

	return p;
}

/*void *krealloc(void *ptr, size_t size)
{
	void *p = kmalloc(size);

	if (!p)
		return NULL;

	memcpy(p, ptr,

	kfree(ptr);

	return p;
}*/

void kfree(void *ptr)
{
	struct addr *cap;

	if (!ptr)
		kprintf("attempted to free NULL ptr\n");

	for (cap = &addresses; ; cap = cap->next) {
		if (cap->addr == ptr) {
			if (cap->present)
				goto found;
			else
				break;
		}
	}

	panic("attempted to free unallocated memory", 0, ptr);

found:
	/* XXX TEMP XXX */ memset(cap->addr, 0, cap->size);
	cap->present = 0;

	/* kprintf("-"); */

	return;
}
#endif

void mm_init(uintptr_t addr, size_t len)
{
	struct mboot_mmap *mmap = (void *) addr;
	size_t mem;

	dprintf("Physical memory map:\n");

	while ((uintptr_t) mmap < addr + len) {
		uintptr_t maddr = mmap->addr_lo | (mmap->addr_hi >> 16);
		uintptr_t mlen = mmap->len_lo | (mmap->len_hi >> 16);

		dprintf(KP_CON "%#018lx - %#018lx (%s)\n",
				maddr, maddr + mlen, mmap_types[mmap->type]);

		mem += mlen;

		mmap = (struct mboot_mmap *)
				((uintptr_t)
				 mmap + mmap->size + sizeof(mmap->size));
	}

	dprintf("%u MB memory\n", mem / 1024 / 1024 + 1);
	/* dprintf("%u kernel size", &kern_end + 0x100000); */

	/* FIXME How much padding is really required? */
	/* position = ((uintptr_t) &kern_end) + 0xE0000; */
	position = 0x300000 + 0xFFFFFFFF80000000;

	/*u16 i;

	for (i = 0; i < BLOCKS_MAX; i++) {
		blocks->allocated = 0;
	}*/
}
