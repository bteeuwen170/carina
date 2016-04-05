/*
 *
 * Carina
 * stdio Library
 *
 * Copyright (C) 2015 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#include <fb.h>
#include <kbd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>
#include <vga.h>

void info(string msg, i8 status, bool print)
{
	if (print) {
		prints("\n");
		prints(msg);

		switch (status) {
			case FAIL:
				prints(" ");
				printsc("FAIL", COLOR_LIGHT_MAGENTA);
				break;
			case OK:
				prints(" ");
				printsc("OK", COLOR_GREEN);
				break;
			/*case FATAL:
				panic(msg, 0);
				break;*/ //TODO Different
		}
	}

	for (u32 i = 0; i < strlen(msg); i++) serial_out(COM0, msg[i]);
	serial_out(COM0, 0x0A);
	serial_out(COM0, 0x0D);
}

void panic(string reason, u64 err_code)
{
	asm volatile ("cli");

	fb_cur_style(CURSOR_GONE);

	prints("\nKernel Panic: ");
	prints(reason); //TODO First do check for specific erro, say page fault or GPF

	if (err_code) {
		if ((err_code >> 1) & 0b00)
			prints(" in GDT.");
		else if ((err_code >> 1) & 0b01)
			prints(" in IDT.");
		else if ((err_code >> 1) & 0b10)
			prints(" in LDT.");
		else if ((err_code >> 1) & 0b11)
			prints(" in IDT.");
	}

	prints("\nError code: 0x");
	prints(itoa(err_code, 16)); //TODO Don't always print error code
	printc('\n');
	prints("The system has been halted.");

	for (;;) asm ("hlt");
}

void printc(char c)
{
	printcc(c, fb_fgcolor);
}

void printcc(char c, u8 color)
{
	if (c != 0) {
		switch (c) {
			case '\b':
				fb_x[fb_focus]--;
				fb_buffer[fb_y[fb_focus] * VGA_WIDTH + fb_x[fb_focus]] =
						vga_create_entry(' ', color, fb_bgcolor); //FIXME New line
				break;
			case '\n':
				fb_x[fb_focus] = 0;
				fb_y[fb_focus]++;
				break;
			case '\r':
				fb_x[fb_focus] = 0; //FIXME Clear line as well!!
			case '\t':
				fb_x[fb_focus] += TAB_SIZE; //FIXME New line
				break;
			default:
				fb_buffer[fb_y[fb_focus] * VGA_WIDTH + fb_x[fb_focus]] =
						vga_create_entry(c, color, fb_bgcolor);
				fb_x[fb_focus]++;
				break;
		}

		if (fb_x[fb_focus] >= VGA_WIDTH) {
			fb_x[fb_focus] = 0;
			fb_y[fb_focus]++;
		}

		if (fb_y[fb_focus] >= VGA_HEIGHT) fb_scrl_dwn();

		fb_cur_set();
	}
}

void prints(string str)
{
	u32 length = 0;
	while (str[length]) printc(str[length++]);
}

void printsc(string str, u8 color)
{
	u32 length = 0;
	while (str[length]) printcc(str[length++], color);
}

/*void scans(char *ptr)
{
	for (;;) {
		if (kbuf == '\n') break;
	}
}*/

void status(i8 status, bool print)
{
	if (print) {
		switch (status) {
			case FAIL:
				prints(" ");
				printsc("FAIL", COLOR_LIGHT_MAGENTA);
				break;
			case OK:
				prints(" ");
				printsc("OK", COLOR_GREEN);
				break;
		}
	}

	//for (u32 i = 0; i < strlen(msg); i++) serial_out(COM0, msg[i]);
	//serial_out(COM0, 0x0A);
	//serial_out(COM0, 0x0D);
}
