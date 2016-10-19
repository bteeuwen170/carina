/*
 *
 * Clemence
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

/* FIXME This is utter crap */
#define BLOCK_SIZE	4096
#define BLOCKS_MAX	(BLOCK_SIZE * sizeof(u8))

extern u64 kern_end;

static vaddr_t position;

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

void mm_init(void)
{
	position = 0x200000;
	/*u16 i;

	for (i = 0; i < BLOCKS_MAX; i++) {
		blocks->allocated = 0;
	}*/
}
