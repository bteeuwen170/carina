/*
 *
 * Elarix
 * kernel/arch/x86/boot/paging32.c
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

#define BASE_OFF	0x200000

#define PAGE_SIZE	4096
#define ENTRY_SIZE	8
#define ENTRIES		PAGE_SIZE / ENTRY_SIZE

#define FLAGS		0b000000000011

#define PTE(x)		get_parent(x, ENTRIES)
#define PDTE(x)		get_parent(PTE(x), ENTRIES)
#define PDPE(x)		get_parent(PDTE(x), ENTRIES)
#define PML4E(x)	get_parent(PDPE(x), ENTRIES)

static u32 get_parent(u32 entry, u32 size)
{
	u32 res = entry / size;

	if (entry % size)
		res++;

	return res;
}

/* TODO Only identity map the kernel binary itself */
void paging_init(struct mboot_info *mboot)
{
	u32 pml4e = 0, pdpte = 0, pdte = 0, pte = 0, pages = 0;
	u32 pdpt_off = 0, pdt_off = 0, pt_off = 0;
#ifdef CONFIG_X86_PAE
	u64 *pml4, *pdpt, *pdt, *pt;
	u64 ac, off;
#else
	u32 *pml4, *pdpt, *pdt, *pt;
	u32 ac, off;
#endif
	u32 i;

	ac = mboot->mem_lo + mboot->mem_hi;

	/* XXX TEMP XXX */
	pages = ((ac > (1 << 22)) ? 1 << 20 : ac >> 2) + 0x100;
	pte = PTE(pages);
	pdte = PDTE(pages);
#ifdef CONFIG_X86_PAE
	pdpte = PDPE(pages);
#endif
#ifdef ARCH_x86_64
	pml4e = PML4E(pages);
#endif

	pdpt_off = pml4e * PAGE_SIZE;
	pdt_off = (pml4e + pdpte) * PAGE_SIZE;
	pt_off = (pml4e + pdpte + pdte) * PAGE_SIZE;

#ifdef ARCH_x86_64
	pml4 = (void *) BASE_OFF;
	off = BASE_OFF + pdpt_off;
	for (i = 0; i < pml4e; i++) {
		pml4[i] = off | FLAGS;
		off += PAGE_SIZE;
	}
#endif

#ifdef CONFIG_X86_PAE
	pdpt = (void *) BASE_OFF + pdpt_off;
	off = BASE_OFF + pdt_off;
	for (i = 0; i < pdpte; i++) {
		pdpt[i] = off | FLAGS;
		off += PAGE_SIZE;
	}
#endif

	pdt = (void *) BASE_OFF + pdt_off;
	off = BASE_OFF + pt_off;
	for (i = 0; i < pdte; i++) {
		pdt[i] = off | FLAGS;
		off += PAGE_SIZE;
	}

	pt = (void *) BASE_OFF + pt_off;
	off = 0;
	for (i = 0; i < pte; i++) {
		pt[i] = off | FLAGS;
		off += PAGE_SIZE;
	}

#if 0
	/* Zero out the whole thing just in case */
	for (i = 0; i < total; i++)
		pml4[i] = 0;

	/* PT Entries (4 KB each) */
	off = FLAGS;
	for (i = 0; i < pages; i++) {
		pt[i] = off;

		/* For BIOS ? */
		/* if (i < 1024)
			pt[i] |= 0x100; */

		off += PAGE_SIZE;
	}

	/* PDT Entries (2 MB each) */
	off = pt_off + FLAGS;
	for (i = 0; i < pte; i++) {
		pdt[i] = off;
		off += PAGE_SIZE;
	}

	/* PDPT Entries (1 GB each) */
	off = pdt_off + FLAGS;
	for (i = 0; i < pdte; i++) {
		pdpt[i] = off;
		off += PAGE_SIZE;
	}

	/* PML4 Entries (512 GB each) */
	off = pdpt_off + FLAGS;
	for (i = 0; i < pdpte; i++) {
		pml4[i] = off;
		off += PAGE_SIZE;
	}
#endif
}
