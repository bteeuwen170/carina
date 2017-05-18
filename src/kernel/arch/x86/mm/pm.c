/*
 *
 * Elarix
 * src/kernel/arch/x86/mm/pm.c
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

#include <asm/cpu.h>
#include <asm/mmap.h>

#include <string.h>

static const char devname[] = "pm";

#define FLAGS_MASK	0xFFFFFFFFFFFFF000

#define PAGE_SIZE	4096
#define FRAME_SIZE	(PAGE_SIZE / sizeof(uintptr_t))

#define PML4		(uintptr_t *) 0xFFFFFF7FBFDFE000
#define PDTP(x)		(uintptr_t *) (0xFFFF000000000000UL + (510UL << 39) + \
		(510UL << 30) + (510UL << 21) + (((x) >> 27) & 0x00001FF000))
#define PDTP_KERN	PDTP(510)
#define PDT(x)		(uintptr_t *) (0xFFFF000000000000UL + (510UL << 39) + \
		(510UL << 30) + (((x) >> 18) & 0x003FFFF000))
#define PT(x)		(uintptr_t *) (0xFFFF000000000000UL + (510UL << 39) + \
		(((x) >> 9) & 0x7FFFFFF000))

#define PML4E(x)	(((x) >> 39) & 511)
#define PDTPE(x)	(((x) >> 30) & 511)
#define PDTE(x)		(((x) >> 21) & 511)
#define PTE(x)		(((x) >> 12) & 511)
#define PAGE(x)		((x) / PAGE_SIZE)

extern uintptr_t kern_end;
static uintptr_t end, off;

uintptr_t virt_to_phys(uintptr_t vaddr)
{
	uintptr_t *pml4 = PML4;
	uintptr_t *pdtp = (uintptr_t *) (pml4[PML4E(vaddr)] & FLAGS_MASK);
	uintptr_t *pdt = (uintptr_t *) (pdtp[PDTPE(vaddr)] & FLAGS_MASK);
	uintptr_t *pt = (uintptr_t *) (pdt[PDTE(vaddr)] & FLAGS_MASK);
	uintptr_t paddr = pt[PTE(vaddr)] & FLAGS_MASK;

	return paddr;
}

/* static void pdtp_alloc(uintptr_t *pml4, uintptr_t vaddr, uintptr_t paddr)
{
	pdtp[PML4E(vaddr)] = paddr | 0b00000011;
	memset(PDTP(vaddr), 0, FRAME_SIZE);
} */

static void pdt_alloc(uintptr_t *pdtp, uintptr_t vaddr, uintptr_t paddr)
{
	pdtp[PDTPE(vaddr)] = paddr | 0b00000011;
	memset(PDT(vaddr), 0, FRAME_SIZE);
}

static void pt_alloc(uintptr_t *pdt, uintptr_t vaddr, uintptr_t paddr)
{
	pdt[PDTE(vaddr)] = paddr | 0b00000011;
	memset(PT(vaddr), 0, FRAME_SIZE);
}

static void page_alloc(uintptr_t *pt, uintptr_t vaddr, uintptr_t paddr)
{
	pt[PTE(vaddr)] = paddr | 0b000000011;
}

static void *_page_alloc(uintptr_t vaddr, uintptr_t paddr)
{
	uintptr_t taddr;

	paddr += off * PAGE_SIZE;

	/* TODO Confirm that this works... */
	if (PDTE(paddr) == 511) {
		taddr = paddr;
		if (!(paddr = mmap_get(end)))
			return NULL;
		paddr += off++ * PAGE_SIZE;
		pdt_alloc(PDTP_KERN, paddr + VM_ADDR, taddr);
	}

	if (PTE(paddr) == 511) {
		taddr = paddr;
		if (!(paddr = mmap_get(end)))
			return NULL;
		paddr += off++ * PAGE_SIZE;

		pt_alloc(PDT(vaddr), paddr + VM_ADDR, taddr);
	}

	page_alloc(PT(vaddr), vaddr, paddr);

#ifdef CONFIG_ALLOC_VERBOSE
	kprintf("O @ %#lx > %#lx < %u < %u < %u < %u\n", vaddr, paddr,
			PTE(vaddr), PDTE(vaddr), PDTPE(vaddr), PML4E(vaddr));
#endif

	return (void *) vaddr;
}

void *page_alloc_user(void)
{
	return NULL;
}

void *page_alloc_kernel(void)
{
	uintptr_t addr;

	if (!(addr = mmap_get(end++)))
		return NULL;

	return _page_alloc(addr + VM_ADDR, addr);
}

void *block_alloc_kernel(size_t size)
{
	uintptr_t pages, pts, pdts;
	uintptr_t addr, caddr, taddr;
	size_t i, j;

	if ((pages = PAGE(size) + (size % PAGE_SIZE ? 1 : 0)) == 1)
		return page_alloc_kernel();

	pts = pages / 512 + (pages % 512 ? 1 : 0);
	pdts = pts / 512 + (pts % 512 ? 1 : 0);

	kprintf("(%u) %u < %u < %u < 1 < 1\n", size, pages, pts, pdts);

	if (!(taddr = mmap_get(end)))
		return NULL;

	/* FIXME We're wasting memory here */
	off += pages + pts + pdts;

	if (!(addr = mmap_get(end)))
		return NULL;
	addr += off * PAGE_SIZE;
	caddr = addr;

	for (i = 0; i < pts; i++) {
		pt_alloc(PDT(caddr + VM_ADDR), caddr + VM_ADDR, taddr);

		for (j = 0; j < pages / pts; j++) {
			/* if (!(caddr = mmap_get(end++)))
				return NULL;
			caddr += off * PAGE_SIZE; */

			page_alloc(taddr, caddr + VM_ADDR, caddr);

			caddr += PAGE_SIZE;
		}

		taddr += PAGE_SIZE;
	}

	return addr;
}

void page_free(void *page)
{
	/* TODO */
}

void pm_init(void)
{
	end = PAGE(((uintptr_t) &kern_end +
			((uintptr_t) &kern_end % PAGE_SIZE) - VM_ADDR));

#ifdef CONFIG_ALLOC_VERBOSE
	dprintf("first page: %#x\n", end);
#endif

	/* TODO Only identity map 1st MB */
}
