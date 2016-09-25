/*
 *
 * Carina
 * src/kernel/dev/kbd/ps2.h
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

#ifndef __PS2_H__
#define __PS2_H__


#define PS2_KEYUP		0x80

#define PS2_CMD			0x64
#define PS2_IO			0x60

#define PS2_RESET		0xFE

#define PS2_EXT			0xE0

/* Modifiers */
#define PS2_LSHIFT		0x2A
#define PS2_LSHIFTU		0xAA
#define PS2_RSHIFT		0x36
#define PS2_RSHIFTU		0xB6
#define PS2_LCTRL		0x1D
#define PS2_LCTRLU		0x9D
#define PS2_RCTRL		0x1DE0
#define PS2_RCTRLU		0x9DE0
#define PS2_LALT		0x38
#define PS2_LALTU		0xB8
#define PS2_RALT		0x38E0
#define PS2_RALTU		0xB8E0
#define PS2_LSUPER		0x5BE0
#define PS2_LSUPERU		0xDBE0
#define PS2_RSUPER		0x5CE0
#define PS2_RSUPERU		0xDCE0

/* Other keys */
/*
 * TODO
 * Esc, PrtSc, CapsLock, NumLock, ScrLk, Pause, Ins, Home, PGUP, Del, End, PGDN,
 * Function keys, Media keys, WinMenu
 */

#endif
