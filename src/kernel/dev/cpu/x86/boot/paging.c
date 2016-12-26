/*
 *
 * Elarix
 * kernel/arch/x86/boot/paging.c
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

//extern u64 kern_end;

//static u64 PML4_OFF;
#define PML4_OFF		0x200000

#define PAGE_SIZE		4096
#define STRUCT_SIZE		PAGE_SIZE
#define ENTRY_SIZE		8
#define ENTRY_CNT		PAGE_SIZE / ENTRY_SIZE

#define BITS			0b00011

static u32 div_round(u32 num, u32 denom)
{
	u32 res = num / denom;

	if (num % denom)
		res++;

	return res;
}

void paging_init(struct mboot_info *mboot)
{
	u32 ac;
	u32 total, pml4e, pdpte, pdte, pte, pages;
	u32 pdpt_off, pdt_off, pt_off;
	u64 *pml4, *pdpt, *pdt, *pt;
	u64 off;
	u32 i;

	/* PML4_OFF = kern_end; */

	ac = mboot->mem_lo + mboot->mem_hi;

	/* Only the first 4GB */
	/* pages	= ((ac > (1 << 22)) ? 1 << 20 : ac >> 2) + 0x100; */
	pages	= (1 << 20) + 0x100;
	pte	= div_round(pages, ENTRY_CNT);
	pdte	= div_round(pte, ENTRY_CNT);
	/* pdpte	= div_round(pdte, ENTRY_CNT); */
	/* pml4e	= div_round(pdpte, ENTRY_CNT); */
	pdpte	= 1; //TEMP
	pml4e	= 1; //TEMP
	total	= pml4e + pdpte + pdte + pte * PAGE_SIZE;

	pml4		= (void *) PML4_OFF;
	pdpt_off	= pml4e * STRUCT_SIZE;
	pdpt		= (void *) PML4_OFF + pdpt_off;
	pdt_off		= (pml4e + pdpte) * STRUCT_SIZE;
	pdt		= (void *) PML4_OFF + pdt_off;
	pt_off		= (pml4e + pdpte + pdte) * STRUCT_SIZE;
	pt		= (void *) PML4_OFF + pt_off;

	off = 0;
	for (i = 0; i < 512; i++) {
		pt[i] = off + BITS;
		off += STRUCT_SIZE;
	}

	off = PML4_OFF + pt_off;
	for (i = 0; i < 512; i++) {
		pdt[i] = off + BITS;
		off += STRUCT_SIZE;
	}

	off = PML4_OFF + pdt_off;
	for (i = 0; i < 512; i++) {
		pdpt[i] = off + BITS;
		off += STRUCT_SIZE;
	}

	off = PML4_OFF + pdpt_off;
	for (i = 0; i < 512; i++) {
		pml4[i] = off + BITS;
		off += STRUCT_SIZE;
	}

	/* Zero out the whole thing just in case */
	//for (i = 0; i < total; i++)
	//	pml4[i] = 0;

	///* PT Entries (4 KB each) */
	//off = BITS;
	//for (i = 0; i < pages; i++) {
	//	pt[i] = off;

	//	/* For BIOS ? */
	//	//if (i < 1024)
	//	//	pt[i] |= 0x100;

	//	off += STRUCT_SIZE;
	//}

	///* PDT Entries (2 MB each) */
	//off = pt_off + BITS;
	//for (i = 0; i < pte; i++) {
	//	pdt[i] = off;
	//	off += STRUCT_SIZE;
	//}

	///* PDPT Entries (1 GB each) */
	//off = pdt_off + BITS;
	//for (i = 0; i < pdte; i++) {
	//	pdpt[i] = off;
	//	off += STRUCT_SIZE;
	//}

	///* PML4 Entries (512 GB each) */
	//off = pdpt_off + BITS;
	//for (i = 0; i < pdpte; i++) {
	//	pml4[i] = off;
	//	off += STRUCT_SIZE;
	//}
}
