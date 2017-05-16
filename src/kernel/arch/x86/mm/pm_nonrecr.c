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
#include <kernel.h> /* XXX TEMP for printing */

#include <asm/cpu.h>
#include <asm/mmap.h>

#include <string.h>

#define FLAGS_MASK	0xFFFFFFFFFFFFF000

#define PAGE_SIZE	4096

#define PML4		0xFFFFFF7FBFDFE000
/* #define PDTP(x)		(0xFFFF000000000000 + (510 << 39) + (510 << 30) + (510 << 21) + (((x) >> 27) & 0x00001FF000))
#define PDT(x)		(0xFFFF000000000000 + (510 << 39) + (510 << 30) + (((x) >> 18) & 0x003FFFF000))
#define PT(x)		(0xFFFF000000000000 + (510 << 39) + (((x) >> 9) & 0x7FFFFFF000)) */

#define PML4E(x)	(((x) >> 39) & 511)
#define PDTPE(x)	(((x) >> 30) & 511)
#define PDTE(x)		(((x) >> 21) & 511)
#define PTE(x)		(((x) >> 12) & 511)
#define PAGE(x)		((x) / PAGE_SIZE)

extern uintptr_t kern_end;
static uintptr_t end, tables, max;

uintptr_t virt_to_phys(uintptr_t addr)
{
	uintptr_t *pml4 = PML4;
	uintptr_t *pdtp = pml4[PML4E(addr)] & FLAGS_MASK;
	uintptr_t *pdt = pdtp[PDTPE(addr)] & FLAGS_MASK;
	uintptr_t *pt = pdt[PDTE(addr)] & FLAGS_MASK;
	uintptr_t p = pt[PTE(addr)] & FLAGS_MASK;

	return p;
}

/* TODO Implement this properly */
uintptr_t phys_to_virt(uintptr_t addr)
{
	return NULL;
	/* return (void *) ((uintptr_t) addr) + VM_ADDR; */
}

static uintptr_t *pdtp_alloc(uintptr_t *pml4, uintptr_t *addr)
{
	if (*addr < VM_ADDR && !(pml4[PML4E(*addr) + 1] & FLAGS_MASK))
		panic("TODO", 0, 0);

#if 0
	if (!(pdtp[PDTPE(addr) + 1] & FLAGS_MASK)) {
		pdtp[PDTPE(addr) + 1] = FLAGS_MASK;
		pdtp[PDTPE(addr) + 1] = (uintptr_t) page_alloc_kernel() | 0b00000011;

		kprintf("new pdt @ %u\n", PDTPE(addr) + 1);
		memset(pdtp[PDTPE(addr) + 1], 0, 512);
	}
#endif

	return (pml4[PML4E(*addr)] & FLAGS_MASK) + VM_ADDR;
}

static uintptr_t *pdt_alloc(uintptr_t *pdtp, uintptr_t *addr)
{
	uintptr_t *pdt;

	pdt = pdtp[PDTPE(*addr)] & FLAGS_MASK;
	/* if (!pdt[PDTE(addr) + 1]) {
		if (!(addr = page_get(end++)))
			return NULL;
		pdt[PDTE(addr) + 1] = (uintptr_t) page_alloc_kernel() | 0b00000011;

		kprintf("new pdt: %u\n", PDTE(addr) + 1);
		memset(pdt[PDTE(addr) + 1], 0, 512);
		for(;;);
	} */

	return (pdtp[PDTPE(*addr)] & FLAGS_MASK) + VM_ADDR;
}
static uintptr_t *page_alloc(uintptr_t *pt, uintptr_t vaddr, uintptr_t paddr);

static uintptr_t *pt_alloc(uintptr_t *pdt, uintptr_t *addr)
{
	uintptr_t pt;

	pt = *addr;
	if (!(*addr = page_get(end++)))
	return NULL;
	*addr += VM_ADDR;

	if (!(pdt[PDTE(*addr)])) {
		kprintf("new addr is %#lx\n", *addr);
		kprintf("creating new pt %u @ %#lx\n", PDTE(*addr), pt);
		kprintf("pt for new pt @ %#lx with index %u\n", (pdt[PDTE(pt)] & FLAGS_MASK) + VM_ADDR, PTE(pt));
		page_alloc((uintptr_t *) (pdt[PDTE(pt)] & FLAGS_MASK), &pt, &pt - VM_ADDR);
		memset(pt, 0, 512);

		pdt[PDTE(*addr)] = (pt - VM_ADDR) | 0b00000011;
		/* kprintf("new pt! %u ptr to %#lx in %#lx\n", PDTE(addr) + 1, pt, pdt);
		for(;;); */
	} else {
		end--;
		*addr = pt;
	}

	return (uintptr_t *) (pdt[PDTE(*addr)] & FLAGS_MASK);
}

static uintptr_t *page_alloc(uintptr_t *pt, uintptr_t vaddr, uintptr_t paddr)
{
	uintptr_t page;

	kprintf("pointing %#lx to %#lx\n", vaddr, paddr);
	if (!(pt[PTE(vaddr)])) {
		sleep(1000);
		/* TEMP */
		pt[PTE(vaddr)] = paddr | 0b100000011;
		kprintf("addr to in pt: %#lx\n", pt);
	}

	return (pt[PTE(vaddr)] & FLAGS_MASK) + VM_ADDR;
}

static void *_page_alloc(uintptr_t paddr, int kernel)
{
	uintptr_t *pdtp, *pdt, *pt;
	uintptr_t vaddr;

	vaddr = paddr + (kernel ? VM_ADDR : 0);

	if (!(pdtp = pdtp_alloc(PML4, &vaddr)))
		panic("out of virtual memory", -ENOMEM, paddr);

	if (!(pdt = pdt_alloc(pdtp, &vaddr)))
		panic("out of virtual memory", -ENOMEM, paddr);

	if (!(pt = pt_alloc(PDT(vaddr), &vaddr)))
		panic("out of virtual memory", -ENOMEM, paddr);

	if (!page_alloc(pt, vaddr, vaddr - VM_ADDR))
		panic("out of virtual memory", -ENOMEM, paddr);

	/* pt[PTE(addr)] = addr | 0b100000011; */

	/* pdt = pdtp[PDTPE(addr + (kernel ? VM_ADDR : 0))] & FLAGS_MASK;
	if (!pdt)
		panic("ENOMEM2", 0, 0);

	pt = pdt[PDTE(addr + (kernel ? VM_ADDR : 0))];
	if (!pt)
		panic("ENOMEM3", 0, 0); */

	/* pdt = pdtp_kern[510];
	pt = pdt[PDTE(addr)]; */
	/* kprintf("%#lx %#lx\n", pdtp_kern, pdtp); */

	/* if (!*pt)
		pt_alloc */

	/* kprintf("pg: %#lx\n", page_get(addr / PAGE_SIZE)); */
	if (vaddr != paddr + VM_ADDR) {
		kprintf("%#lx > %#lx\n", 0xffffffff801ff000, virt_to_phys(0xffffffff801ff000));
		kprintf("%#lx > %#lx\n", 0xffffffff80200000, virt_to_phys(0xffffffff80200000));
		kprintf("%#lx > %#lx\n", 0xffffffff80201000, virt_to_phys(0xffffffff80201000));
	}

/* #ifdef CONFIG_ALLOC_VERBOSE */
	kprintf("O @ %#lx > %#lx < %u < %u < %u < %u\n", vaddr, paddr,
			PTE(vaddr), PDTE(vaddr), PDTPE(vaddr), PML4E(vaddr));
/* #endif */

	return (void *) vaddr;
}

void *page_alloc_user(void)
{
	return NULL;
}

void *page_alloc_kernel(void)
{
#if 1
	uintptr_t addr;

	if (end > max - tables)
		return NULL;

	if (!(addr = page_get(end++)))
		return NULL;

	kprintf("%#lx\n", virt_to_phys(0x100000 + VM_ADDR));

	return _page_alloc(addr, 1);
#else
	uintptr_t *pt, paddr, addr, _pt;
	int i;

	if (!(paddr = page_get(end++)))
		return NULL;

	/* if (!*(pdt_kern + PDTE(addr))) {
		kprintf("!");
		_pt = _page_alloc_kernel();
		for(;;);
		*(pdt_kern + PDTE(addr) + 1) = _pt;
		pt = _pt + VM_ADDR;
		for (i = 0; i < TABLE_SIZE; i++)
			*pt++ = 0;
	} */

	page_alloc(paddr);

	return (void *) paddr + VM_ADDR;
#endif
}

void page_free(void *page)
{

}

void pm_init(void)
{
	/* uintptr_t *pdt, addr;
	int i; */

	end = PAGE(((uintptr_t) &kern_end +
			((uintptr_t) &kern_end % PAGE_SIZE) - VM_ADDR));
	max = page_max() - 1;

#ifdef CONFIG_ALLOC_VERBOSE
	dprintf("first page: %#x\n", end);
	dprintf("max pages: %u\n", max);
#endif

	/* pdt = PT((end - 4) * 4096);
	if (pdt[PTE((end - 4) * 4096)] & 0x01)
		panic("help", 0, 0); */

	/* pdt = PDT(0);
	for (i = 1; i < 3; i++)
		pdt[i] &= ~1; */
	/* memset(pdt[PDTE(0x100000)], 0, 256); */
	/* pdt_lo[PDTE(0x100000] = 0x */
}
