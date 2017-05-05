/*
 *
 * Elarix
 * src/kernel/arch/x86/boot/link.ld
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

VM_ADDR = 0xC0000000;

SECTIONS
{
	. = 0x100000;

	.mboot ALIGN(4096) : {
		*(.mboot)
		. = ALIGN(4096);
	}

	. += VM_ADDR;

	.text ALIGN(4096) : AT(ADDR(.text) - VM_ADDR) {
		*(.text)
	}

	.data ALIGN(4096) : AT(ADDR(.data) - VM_ADDR) {
		*(.data)
		*(.rodata)
	}

	.bss ALIGN(4096) : AT(ADDR(.bss) - VM_ADDR) {
		*(COMMON)
		*(.bss)
	}

	kern_end = .;

	/DISCARD/ : {
		*(.eh_frame)
		*(.comment)
	}
}
