/*
 *
 * Elarix
 * src/kernel/arch/x86/include/asm/lock.h
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

#ifndef _X86_LOCK_H
#define _X86_LOCK_H

#if 1
#define spin_lock(name) \
	while (!__sync_bool_compare_and_swap(&name, 0, 1)); \
		__sync_synchronize();

#define spin_unlock(name) \
	__sync_synchronize(); \
	name
#else
static inline void spin_lock(spinlock_t *lock)
{
	(void) lock;
	if (lock)
		panic("attempted to lock a locked spinlock", 0, 0);

	asm volatile ("lock; xchgl %0, %1" : "+m" (*lock) : "1" (1) : "cc");

	__sync_synchronize();
}

static inline void spin_unlock(spinlock_t *lock)
{
	(void) lock;
	if (!lock)
		panic("attempted to unlock a unlocked spinlock", 0, 0);

	__sync_synchronize();

	asm volatile ("movl $0, %0" : "+m" (lock) : );
}
#endif

#endif
