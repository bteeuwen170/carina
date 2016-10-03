/*
 *
 * Carina
 * src/kernel/arch/x86/cpu/cpu.c
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

#include <cpu.h>
#include <stdio.h>
#include <string.h>

/* CRAP */
void cpu_info(void)
{
	u32 eax, ebx, ecx, edx;
	char brand[48];

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

	char *name = brand;
	while (*name == ' ')
		++name; //TODO Right and left justify function

	prints(name);
}
