/*
 *
 * Elara
 * src/kernel/arch/x86/include/asm/lock.h
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

#ifndef _X86_LOCK_H
#define _X86_LOCK_H

static inline void spin_lock(spinlock_t lock)
{
#if 0
	asm volatile(
			"lock \n" \
			"decl %0 \n" \
			"js 2 \n" \
			"2: \n" \
			"cmpl $0, %0 \n" \
			"rep \n" \
			"nop \n" \
			"jle 2 \n" \
			"jmp 1"
			: "=m" (lock) : : "memory");
#else
	(void) lock;
#endif
}

static inline void spin_unlock(spinlock_t lock)
{
#if 0
	asm volatile("movl $1, %0" : "=m" (lock) : : "memory");
#else
	(void) lock;
#endif
}

#endif
