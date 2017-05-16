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

#define FLAGS_MASK	0xFFFFFFFFFFFFF000

#define PAGE_SIZE	4096

#define PML4		0xFFFFFF7FBFDFE000
#define PDTP(x)		(0xFFFF000000000000 + (510 << 39) + (510 << 30) + (510 << 21) + (((x) >> 27) & 0x00001FF000))
#define PDT(x)		(0xFFFF000000000000 + (510 << 39) + (510 << 30) + (((x) >> 18) & 0x003FFFF000))
#define PT(x)		(0xFFFF000000000000 + (510 << 39) + (((x) >> 9) & 0x7FFFFFF000))

#define PML4E(x)	(((x) >> 39) & 511)
#define PDTPE(x)	(((x) >> 30) & 511)
#define PDTE(x)		(((x) >> 21) & 511)
#define PTE(x)		(((x) >> 12) & 511)
#define PAGE(x)		((x) / PAGE_SIZE)

extern uintptr_t kern_end;
static uintptr_t end;

static uintptr_t *pdtp_alloc(uintptr_t *pml4, uintptr_t addr)
{
	if (addr < VM_ADDR && !(pml4[PML4E(addr) + 1] & FLAGS_MASK))
		panic("TODO", 0, 0);

#if 0
	if (!(pdtp[PDTPE(addr) + 1] & FLAGS_MASK)) {
		pdtp[PDTPE(addr) + 1] = FLAGS_MASK;
		pdtp[PDTPE(addr) + 1] = (uintptr_t) page_alloc_kernel() | 0b00000011;

		kprintf("new pdt @ %u\n", PDTPE(addr) + 1);
		memset(pdtp[PDTPE(addr) + 1], 0, 512);
	}
#endif

	return pml4[PML4E(addr)] & FLAGS_MASK;
}

static uintptr_t *pdt_alloc(uintptr_t *pdtp, uintptr_t addr)
{
	uintptr_t *pdt;

	pdt = pdtp[PDTPE(addr)] & FLAGS_MASK;
	kprintf("pdtpaddr %#lx\n", pdtp, PDTPE(addr));
	/* for(;;); */
	if (!pdt[PDTE(addr) + 2]) {
		pdt[PDTE(addr) + 2] = FLAGS_MASK;
		pdt[PDTE(addr) + 2] = (uintptr_t) page_alloc_kernel() | 0b00000011;

		kprintf("new pdt: %#x\n", pdt[PDTE(addr) + 1] & FLAGS_MASK);
		memset(pdt[PDTE(addr) + 2], 0, 512);
		/* for(;;); */
	}

	return pdtp[PDTPE(addr)] & FLAGS_MASK;
}

static uintptr_t *pt_alloc(uintptr_t *pdt, uintptr_t addr)
{
	if (!(pdt[PDTE(addr) + 2] & FLAGS_MASK)) {
		pdt[PDTE(addr) + 2] = FLAGS_MASK;
		pdt[PDTE(addr) + 2] = (uintptr_t) page_alloc_kernel() | 0b00000011;

		kprintf("new pt: %#x\n", pdt[PDTE(addr)] & FLAGS_MASK);
		memset(pdt[PDTE(addr) + 2], 0, 512);

		uintptr_t *pt = pdt[PDTE(addr) + 2] & FLAGS_MASK;
		int i;
		for(i = 0; i < 512;i++) {

			pt[i] = (uintptr_t) page_alloc_kernel() | 0b100000011;
		}
		for(;;);
		/* pt[0] = FLAGS_MASK;
		pt[0] = ;
		pt[1] = FLAGS_MASK;
		pt[1] = (uintptr_t) page_alloc_kernel() | 0b100000011; */
	}

	return pdt[PDTE(addr)] & FLAGS_MASK;
}

static uintptr_t *page_alloc(uintptr_t *pt, uintptr_t addr)
{
	/* if (PTE(addr) < 511 && !(pt[PTE(addr) + 1] & FLAGS_MASK)) {
		pt[PTE(addr) + 1] = FLAGS_MASK;
		pt[PTE(addr) + 1] = (uintptr_t) page_alloc_kernel() | 0b100000011;
		kprintf("new page: %#x\n", pt[PTE(addr)] & FLAGS_MASK);

		memset(pt[PTE(addr) + 1], 0, 512);
	} */

	return pt[PTE(addr)] & FLAGS_MASK;
}

static void *_page_alloc(uintptr_t addr, int kernel)
{
	uintptr_t *pdtp, *pdt, *pt, *page;

	if (!(pdtp = pdtp_alloc(PML4, addr + (kernel ? VM_ADDR : 0))))
		panic("out of virtual memory", -ENOMEM, addr);

	if (!(pdt = pdt_alloc(pdtp, addr + (kernel ? VM_ADDR : 0))))
		panic("out of virtual memory", -ENOMEM, addr);

	if (!(pt = pt_alloc(pdt, addr + (kernel ? VM_ADDR : 0))))
		panic("out of virtual memory", -ENOMEM, addr);

	if (!(page = page_alloc(pt, addr + (kernel ? VM_ADDR : 0))))
		panic("out of virtual memory", -ENOMEM, addr);

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

/* #ifdef CONFIG_ALLOC_VERBOSE */
	kprintf("O @ %#x < %u < %u < %u < %u\n", addr,
			PTE(addr + (kernel ? VM_ADDR : 0)),
			PDTE(addr + (kernel ? VM_ADDR : 0)),
			PDTPE(addr + (kernel ? VM_ADDR : 0)),
			PML4E(addr + (kernel ? VM_ADDR : 0)));
/* #endif */

	return (void *) addr + (kernel ? VM_ADDR : 0);
}

void *page_alloc_user(void)
{
	return NULL;
}

void *page_alloc_kernel(void)
{
#if 1
	uintptr_t addr;

	if (!(addr = page_get(end++)))
		return NULL;

	return addr + VM_ADDR;
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

/* TODO Implement these properly and use them */
void *virt_to_phys(void *addr)
{
	return (void *) ((uintptr_t) addr) - VM_ADDR;
}

void *phys_to_virt(void *addr)
{
	return (void *) ((uintptr_t) addr) + VM_ADDR;
}

void pm_init(void)
{
	uintptr_t *pdt, addr;
	int i;

	end = PAGE(((uintptr_t) &kern_end +
			((uintptr_t) &kern_end % PAGE_SIZE) - VM_ADDR));

#ifdef CONFIG_ALLOC_VERBOSE
	dprintf("first page: %#x\n", end);
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
