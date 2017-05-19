/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/cpu.c
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

#include <cmdline.h>
#include <kernel.h>
#include <mboot.h>

#include <asm/cpu.h>
#include <asm/mmap.h>
#include <asm/pic.h>

#include <string.h>

/* TODO Move */
extern void early_kprint_init(void);

#if 0
static void cpu_info(void)
{
	u32 eax, ebx, ecx, edx;
	char brand[48], *name;

	cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
	memcpy(brand, &eax, 4);
	memcpy(brand + 4, &ebx, 4);
	memcpy(brand + 8, &ecx, 4);
	memcpy(brand + 12, &edx, 4);

	cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
	memcpy(brand + 16, &eax, 4);
	memcpy(brand + 20, &ebx, 4);
	memcpy(brand + 24, &ecx, 4);
	memcpy(brand + 28, &edx, 4);

	cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
	memcpy(brand + 32, &eax, 4);
	memcpy(brand + 36, &ebx, 4);
	memcpy(brand + 40, &ecx, 4);
	memcpy(brand + 44, &edx, 4);

	name = brand;
	while (*name == ' ')
		++name; /* TODO Right and left justify function */

	kprintf(KP_DBG "%s\n", name);
}
#endif

void cpu_init(struct mboot_info *_mboot)
{
	struct mboot_info mboot;

	memcpy(&mboot, _mboot, sizeof(struct mboot_info));

	idt_init();

	early_kprint_init();
	cmdline_init((const char *) (uintptr_t) mboot.cmdline + VM_ADDR);

	mmap_init(mboot.mmap_addr + VM_ADDR, mboot.mmap_len);
	pm_init();
	mm_init();

	pic_init();

	/* cpu_info() */

	kernel_main();
}
