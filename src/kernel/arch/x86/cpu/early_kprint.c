/*
 *
 * Elarix
 * src/kernel/arch/x86/cpu/early_kprint.c
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

static u16 *vga = (u16 *) 0xB8000;
static u8 x, y;

/* FIXME Needs a better eco system */
void early_kprint(const char *buf, size_t n)
{
	return;

	while (n--) {
		if (*buf == '\n') {
			x = 0;
			y++;
			buf++;
			continue;
		}

		vga[y * 80 + x++] = ((u16) *buf++) | ((u16) 0x07) << 8;
	}
}
