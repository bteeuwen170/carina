/*
 *
 * Elarix
 * src/kernel/link.ld
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

LOAD_VMA = 0x100000;
/* KERN_VMA = 0xFFFFFFFF80000000; */

SECTIONS
{
	. = LOAD_VMA;

	.mboot ALIGN(4096) : {
		*(.mboot)
		. = ALIGN(4096);
	}

	.text ALIGN(4096) : AT(ADDR(.text)) {
		*(.text)
	}

	.data ALIGN(4096) : AT(ADDR(.data)) {
		*(.data)
		*(.rodata)
	}

	.bss ALIGN(4096) : AT(ADDR(.bss)) {
		*(COMMON)
		*(.bss)
	}

	/* XXX Do we need any padding or alternative alignment? */
	kern_end = .;

	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
	}
}
