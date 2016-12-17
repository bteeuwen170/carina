/*
 *
 * Elara
 * src/kernel/lib/stdio.c
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

#include <print.h>

#include <kbd/kbd.h>
#include <video/vga.h>

#include <stdio.h>
#include <string.h>

/*
 * TODO This has to be safer
 * Don't use kprintf, switch to VGA if double fault, etc...
 * TODO Also relocate in other file
 * TODO Seperate panic for isrs
 * TODO Dump registers (at least rip/eip)
 */
void panic(char *reason, u32 err_code, intptr_t ip)
{
	asm volatile ("cli");

	//TODO Hide cursor

	kprintf(KP_CRIT, "panic", "%s @ %#x\n", reason, ip);

	/* TODO Don't always print error code */
	kprintf(KP_CRIT, "panic", "Error code: %#x\n", err_code);

	prints("The system has been halted.");

	for (;;)
		asm volatile ("hlt");
}

void printc(char c)
{
	printcc(c, vga_fgcolor);
}

void printcc(char c, u8 color)
{
	vga_putch(c, color);
}

void prints(char *str)
{
	while (*str)
		printc(*(str++));
}
