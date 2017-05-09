/*
 *
 * Elarix
 * src/kernel/dev/kbd/ps2.c
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

/*
 * TODO Have this driver support multiple ps2 keyboards,
 * and detect devices by scanning the ps2 bus
 */

#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <input.h>
#include <ioctl.h>
#include <kernel.h>
#include <module.h>

#include <asm/cpu.h>

static const char devname[] = "ps2kbd";

#define KEYMAP_SIZE 128

#define PS2_KEYUP	0x80
#define PS2_CMD		0x64
#define PS2_IO		0x60
#define PS2_RESET	0xFE
#define PS2_EXT		0xE0

#define PS2_LSHIFT	0x2A
#define PS2_LSHIFTU	0xAA
#define PS2_RSHIFT	0x36
#define PS2_RSHIFTU	0xB6
#define PS2_LCTRL	0x1D
#define PS2_LCTRLU	0x9D
#define PS2_RCTRL	0x1DE0
#define PS2_RCTRLU	0x9DE0
#define PS2_LALT	0x38
#define PS2_LALTU	0xB8
#define PS2_RALT	0x38E0
#define PS2_RALTU	0xB8E0
#define PS2_LSUPER	0x5BE0
#define PS2_LSUPERU	0xDBE0
#define PS2_RSUPER	0x5CE0
#define PS2_RSUPERU	0xDCE0

#define MOD_SHIFT	0x01
#define MOD_CTRL	0x02
#define MOD_SUPER	0x04
#define MOD_ALT		0x08

/*
 * TODO
 * Esc, PrtSc, CapsLock, NumLock, ScrLk, Pause, Ins, Home, PGUP, Del, End, PGDN,
 * Function keys, Media keys, WinMenu
 */

/* TODO Map this to keys defined in input.h instead of characters themselves */
static const char kbd_keymap[KEYMAP_SIZE] = {
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

static const char kbd_keymap_alt[KEYMAP_SIZE] = {
	-1,
	0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*',
	0, ' ',
	0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	0, 0, 0, '-',
	0, 0, 0, '+',
	0, 0, 0,
	0, 0
};

static volatile struct input_event ps2kbd_ie;
static u8 modifiers, skip;

static int int_handler(struct int_stack *regs)
{
	u16 scancode;
	(void) regs;

	scancode = io_inb(PS2_IO);

	if (skip) {
		skip--;
		return 1;
	}

	if (scancode == PS2_EXT) {
		skip++;
		scancode |= io_inb(PS2_IO) << 8;
	} else {
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
				ps2kbd_ie.code = KEY_RELEASE;
			else
				ps2kbd_ie.code = KEY_PRESS;

			if (modifiers & MOD_SHIFT)
				ps2kbd_ie.value = kbd_keymap_alt[scancode];
			else
				ps2kbd_ie.value = kbd_keymap[scancode];
		}
	}

	return 1;
}

static int ps2kbd_ioctl(struct file *fp, unsigned int cmd, va_list args)
{
	struct input_event *iep;
	(void) fp;

	if (cmd != IO_KEYEV)
		return -EINVAL;

	iep = va_arg(args, struct input_event *);

	ps2kbd_ie.code = KEY_NONE;
	while (ps2kbd_ie.code == KEY_NONE)
		asm volatile ("hlt");
	*iep = ps2kbd_ie;

	return 0;
}

static struct file_ops ps2kbd_file_ops = {
	.ioctl = &ps2kbd_ioctl
};

static int ps2kbd_probe(struct device *devp)
{
	devp->op = &ps2kbd_file_ops;

	return irq_handler_reg(IRQ_KBD, &int_handler);
}

static void ps2kbd_fini(struct device *devp)
{
	(void) devp;

	irq_handler_unreg(IRQ_KBD);
}

static struct driver ps2kbd_driver = {
	.name	= devname,
	.major	= MAJOR_KBD,

	.busid	= BUS_NONE,
	.bus	= NULL,

	.probe	= &ps2kbd_probe,
	.fini	= &ps2kbd_fini
};

int ps2kbd_init(void)
{
	struct device *devp;
	int res;

	if ((res = driver_reg(&ps2kbd_driver)) < 0)
		return res;

	if ((res = device_reg(&ps2kbd_driver, &devp, 0)) < 0)
		return res;
	devp->name = "PS/2 keyboard";

	return 0;
}

void ps2kbd_exit(void)
{
	driver_unreg(&ps2kbd_driver);
}

MODULE(ps2kbd, &ps2kbd_init, &ps2kbd_exit);
