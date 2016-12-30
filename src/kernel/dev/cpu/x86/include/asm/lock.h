/*
 *
 * Elarix
 * src/kernel/dev/cpu/x86/include/asm/lock.h
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

static inline void spin_lock(spinlock_t *lock)
{
	/* if (lock)
		panic("attempted to lock a locked spinlock");

	asm volatile ("lock; xchgl %0, %1" : "+m" (*lock) : "1" (1) : "cc");

	__sync_synchronize(); */
}

static inline void spin_unlock(spinlock_t *lock)
{
	/* if (!lock)
		panic("attempted to unlock a unlocked spinlock");

	__sync_synchronize();

	asm volatile ("movl $0, %0" : "+m" (lock) : ); */
}

#endif
