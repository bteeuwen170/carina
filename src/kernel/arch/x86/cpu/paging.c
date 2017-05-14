/*
 *
 * Elarix
 * kernel/arch/x86/boot/paging.c
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

#include <kernel.h> /* XXX TEMP for printing */

#include <asm/cpu.h>

#define PAGE_MASK	0xFFFFFFFFFFFFF000

extern uintptr_t _pml4;

void *page_alloc_user(void)
{
	return NULL;
}

void *page_alloc_kernel(void)
{

}

void page_free(void *page)
{

}

void *virt_to_phys(void *addr)
{
	return (void *) ((uintptr_t) addr) - VM_ADDR;
}

void *phys_to_virt(void *addr)
{
	return (void *) ((uintptr_t) addr) + VM_ADDR;
}

void paging_init(void)
{
	uintptr_t *pml4, *pdpt, *pdt;
	int i, j;

	/* TODO Undo kernel 1 - 6 MB identity mapping */

	pml4 = &_pml4;
	pdpt = (*pml4 + VM_ADDR) & PAGE_MASK;
	pdt = (*pdpt + VM_ADDR) & PAGE_MASK;

	/* uintptr_t *ptr = *(pdt + (VM_ADDR & PAGE_MASK) + 3 * 8);

	*(ptr */

	/* uintptr_t *ptr = (uintptr_t *) (1536 * 4096);

	kprintf("%#x\n", pdt);
	*(pdt + 4) = ((uintptr_t) ptr) | 0b00000011;

	for (j = 0; j < 512; j++)
		*ptr++ = ((2048 + j) * 4096) | 0b00000011; */

	kprintf("pml4: %#x\n", pml4);
	kprintf("pdpt: %#x\n", pdpt);
	kprintf("pdt: %#x\n", pdt);
	kprintf("pte base: %#x\n", 1536 * 4096);
	return;

	for (i = 4; i < 10; i++) {
		uintptr_t *ptr = (uintptr_t *) ((1536 + i - 4) * 4096);

		*(pdt + i) = ((uintptr_t) ptr) + 0b00000011;

		for (j = 0; j < 512; j++)
			*ptr++ = ((2048 + j) * 4096) + 0b100000011;
	}

	/* *pdpt &= ~1;
	*pml4 &= ~1; */
}
