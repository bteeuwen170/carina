/*
 *
 * Elara
 * src/kernel/arch/x86/include/asm/sys/types.h
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

#ifndef _X86_TYPES_H
#define _X86_TYPES_H

typedef signed char		__asm_i8;
typedef signed short		__asm_i16;
typedef signed int		__asm_i32;
typedef signed long long	__asm_i64;

typedef unsigned char		__asm_u8;
typedef unsigned short		__asm_u16;
typedef unsigned int		__asm_u32;
typedef unsigned long long	__asm_u64;

typedef long		__asm_ssize_t;
typedef unsigned long	__asm_size_t;

typedef long		__asm_intptr_t;
typedef unsigned long	__asm_uintptr_t;

#endif
