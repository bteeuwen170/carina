/*
 *
 * Carina
 * PS/2 Keyboard Driver
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

#include <stdlib.h>

#include <kbd.h>
#include <stdio.h> //TEMP
#include <system.h>

const char kbd_keymap[KEYMAP_SIZE] = { //TODO To header
	0, 27,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*',
	0,
	' ',
	0,
	0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0,
	0,
	0,
	'-',
	0,
	0,
	0,
	'+',
	0,
	0,
	0,
	0,
	0,
	0, 0, 0,
	0,
	0,
	0,
};

const char kbd_keymap_shift[KEYMAP_SIZE] = {
	0   ,   27,
	0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, '\b',
	'\t', '\t',
	0,
	0x61, 0x73, 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B,
	'\'', 0x60, 0,
	'\\', 0x7A, 0x78, 0x63, 0x76, 0x62, 0x6E, 0x6D, 0x2C, 0x2E,
	0,
	0x2A,
	0,
	0x20,
	0,
	0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0,
	0,
	0,
	0x3D,
	0,
	0,
	0,
	0x2B,
	0,
	0,
	0,
	0,
	0,
	0, 0, 0,
	0,
	0,
	0,
};

void kbd_in(registers_t *registers)
{
	(void) registers;
	uint8 scancode = io_inb(KBD_IO);

	if (scancode & KBD_KEY_UP) {
		switch (scancode) {
			case (KBD_KEY_UP & 0x58):
				//prints("CAPS");
				break;
			default:
				//prints("I");
				break;
		}
	} else {
		//kbuf = kbd_keymap[scancode];
		printc(kbd_keymap[scancode]);
	}

}

void kbd_init(void)
{
	//TODO Numlock and such
}

void kbd_enable(void)
{
	irq_register(IRQ_KBD, & kbd_in);
}

void kbd_disable(void)
{
	irq_deregister(IRQ_KBD);
}
