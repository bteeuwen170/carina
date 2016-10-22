/*
 *
 * Clemence
 * src/kernel/arch/x86/include/asm/8259.h
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

#ifndef _X86_8259_H
#define _X86_8259_H

#define PIC_M_CMD	0x20
#define PIC_M_IO	0x21
#define PIC_M_OFF	0x20

#define PIC_S_CMD	0xA0
#define PIC_S_IO	0xA1
#define PIC_S_OFF	0x28

#define PIC_EOI		0x20

#define ICW1_PIC_M	0x02
#define ICW1_PIC_S	0x04

#define ICW1_INIT	0x11
#define ICW1_8086	0x01

void pic_remap(void);

void pic_disable(void);

#endif
