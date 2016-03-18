/*
 *
 * Carina
 * Main Cito Kernel
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


#include <cito.h>
#include <issue.h>
#include <fb.h>
#include <kbd.h>
#include <multiboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>
#include <vga.h>

void kernel_panic(string reason, uint64 err_code)
{
	asm volatile ("cli");
	
	fb_cur_style(CURSOR_GONE);

	prints("\nKernel Panic: ");
	prints(reason);
	prints("Error code: 0x");
	prints(itoa(err_code, 16)); //TODO Don't always print error code
	printc('\n');
	prints("The system has been halted.");

	for (;;) asm ("hlt");
}

void kernel_status(string type, string message, const bool print)
{
	if (print) {
		if (type) {
			printc('[');
			printsc(type, COLOR_LIGHT_MAGENTA);
			prints("] ");
		} else {
			prints("      ");
		}

		prints(message);
		prints("\n");
	}

	for (uint32 i = 0; i < 18; i++) serial_out(COM0, message[i]);
	serial_out(COM0, 0x0A);
	serial_out(COM0, 0x0D); //What the fuck, this ain't Windows right?
	/* !!!HACK!!! */
	//TODO In seperate function
	//FIXME Get size of message and not of pointer
	//TODO Dmesg > kmesg
}

void kernel_main(mbis_t *mbis)
{
	fb_cur_style(CURSOR_GONE);
	serial_init(COM0); //TODO Is this even required?
	fb_init(0);
	idt_init();
	pit_init();

	asm volatile ("sti");

	//unsigned eax, edx;
	//eax = 1;
	//cpuid(&eax, &ebx, &ecx, &edx); //TODO Needs interrupts?
	//prints((eax >> 4) & 0xF);

	/*printsc("!\nLocal compile date and time: ", COLOR_WHITE);
	printsc(__DATE__, COLOR_WHITE);
	printc(' ');
	printsc(__TIME__, COLOR_WHITE);*/ //TODO Maybe in klog but not here

	printsc("Starting ", COLOR_WHITE);
	printsc("Carina ", COLOR_LIGHT_BLUE);
	//printlc(CARINA_VER, COLOR_WHITE);
	kernel_status(0, "Welcome to Carina!", false); //TODO Display version

	printsc("on ", COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_CENTURY), 16), COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_YEAR), 16), COLOR_WHITE);
	printcc('/', COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_MONTH), 16), COLOR_WHITE);
	printcc('/', COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_DAY), 16), COLOR_WHITE);
	printc(' ');
	printsc(itoa(cmos_in(CMOS_HOURS), 16), COLOR_WHITE);
	printcc(':', COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_MINUTES), 16), COLOR_WHITE);
	printcc(':', COLOR_WHITE);
	printsc(itoa(cmos_in(CMOS_SECONDS), 16), COLOR_WHITE);
	printsc(" UTC\nTotal RAM: ", COLOR_WHITE);

	uint64 mem = mbis-> mem_lower + mbis->mem_higher;
	printsc(itoa(mem / 1024 + 1, 10), COLOR_WHITE); //TODO Don't do + 1
	printsc(" MB (", COLOR_WHITE);
	printsc(itoa(mem, 10), COLOR_WHITE);
	printsc(" KB)\n", COLOR_WHITE);

	//pcspk_play(90);
	//sleep(50);
	//pcspk_stop();

	// <TEMP>
	prints("\n$ ");
	fb_cur_style(CURSOR_FLAT);
	kbd_enable();
	//usrmode_enter();
	//for (;;) asm volatile ("hlt");
	for (;;) {
		for (;;) {
			if (kbuf == '\n') {
				break;
			} else if (kbuf != 0) {
	//sleep(100);
				printc(kbuf);
				kbuf = 0;
			}
		}

		kbuf = 0;

		prints("\nCommand not found! Uptime: ");
		//prints(itoa(uptime() * 10, 10));
		prints(" s.\n$ ");
	}
	// </TEMP>
	//char *input;
	//scans(input);
	//kbd_init();
	for(;;) asm volatile("hlt");

	//kbd_get_input(INPUT_LINE);
	//init_printf(0, & printc);
	//printf("df");
}
