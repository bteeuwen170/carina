/*
 *
 * Carina
 * src/kernel/dev/kbd/ps2.c
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

#include <asm/cpu.h>

#include "kbd.h"
#include "ps2.h"

#define KEYMAP_SIZE 128

static char kbuf = -1;
/* TODO Make this a pipe */

static u8 modifiers, skip;

const char kbd_keymap[KEYMAP_SIZE] = {
	-1,
	0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*',
	0, ' ',
	0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	'7', '8', '9', '-',
	'4', '5', '6', '+',
	'1', '2', '3',
	'0', '.'
};

const char kbd_keymap_alt[KEYMAP_SIZE] = {
	/* Shift */
	-1,
	0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	/* Num lock */
	'*',
	0, ' ',
	0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	0, 0, 0, '-',
	0, 0, 0, '+'
};

static void int_handler(struct int_stack *regs)
{
	(void) regs;
	u16 scancode = io_inb(PS2_IO);

	if (skip) {
		skip--;
		return;
	}

	if (scancode == PS2_EXT) {
		skip++;
		scancode |= io_inb(PS2_IO) << 8;
	}

	/* TODO Add escape sequences */

	switch (scancode) {
	case PS2_LSHIFT:
	case PS2_RSHIFT:
		modifiers |= MOD_SHIFT;
		break;
	case PS2_LSHIFTU:
	case PS2_RSHIFTU:
		modifiers &= ~MOD_SHIFT;
		break;
	case PS2_LCTRL:
	case PS2_RCTRL:
		modifiers |= MOD_CTRL;
		break;
	case PS2_LCTRLU:
	case PS2_RCTRLU:
		modifiers &= ~MOD_CTRL;
		break;
	case PS2_LALT:
	case PS2_RALT:
		modifiers |= MOD_ALT;
		break;
	case PS2_LALTU:
	case PS2_RALTU:
		modifiers &= ~MOD_ALT;
		break;
	case PS2_LSUPER:
	case PS2_RSUPER:
		modifiers |= MOD_SUPER;
		break;
	case PS2_LSUPERU:
	case PS2_RSUPERU:
		modifiers &= ~MOD_SUPER;
		break;
	default:
		if (scancode & PS2_KEYUP)
			break;

		if (modifiers & MOD_SHIFT)
			kbuf = kbd_keymap_alt[scancode];
		else
			kbuf = kbd_keymap[scancode];
	}

}

void kbd_init(void)
{
	//TODO Numlock and such
}

void kbd_enable(void)
{
	modifiers = 0;

	irq_reghandler(IRQ_KBD, &int_handler);
}

void kbd_disable(void)
{
	irq_unreghandler(IRQ_KBD);
}

/* TODO Not here, please this os is a mess ... */
char getch(void)
{
	kbuf = -1;

	while (kbuf == -1)
		asm volatile ("hlt");

	return kbuf;
}
