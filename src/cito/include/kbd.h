/*
 *
 * Carina
 * Header for PS/2 Keyboard Driver
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

#ifndef __KBDPS2_H__
#define __KBDPS2_H__

#include <stdint.h>

#define KEYMAP_SIZE		128

#define KBD_KEY_UP		0x80

#define KBD_CMD			0x64
#define KBD_IO			0x60

#define KBD_RESET		0xFE

enum INPUT_LENGTH {
	INPUT_CHAR			= 0x01,
	INPUT_LINE			= 0x02
};

char kbuf;

//void kbd_init(void);

void kbd_enable(void);

void kbd_disable(void);

#endif
