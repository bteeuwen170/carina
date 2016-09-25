/*
 *
 * Carina
 * src/kernel/arch/x86/include/div64.h
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

#ifndef __DIV64_H_
#define __DIV64_H_

/*
#define do_div(n, base)														\
({																			\
	u32 __upper, __low, __high, __mod, __base;								\
	__base = (base);														\
 																			\
	if (__builtin_constant_p(__base) && is_power_of_2(__base)) {			\
		__mod = n & (__base - 1);											\
		n >>= ilog2(__base);												\
	} else {																\
		asm("" : "=a" (__low), "=d" (__high) : "A" (n));					\
		__upper = __high;													\
 																			\
		if (__high) {    													\
			__upper = __high % (__base);									\
			__high = __high / (__base);										\
		}																	\
 																			\
		asm("divl %2" : "=a" (__low), "=d" (__mod)							\
				: "rm" (__base), "" (__low), "1" (__upper));				\
		asm("" : "=A" (n) : "a" (__low), "d" (__high));						\
	}																		\
	__mod;																	\
})
*/

#define do_div(n, base)														\
({																			\
	u32 __upper, __low, __high, __mod, __base;								\
	__base = (base);														\
 																			\
	asm("" : "=a" (__low), "=d" (__high) : "A" (n));						\
	__upper = __high;														\
 																			\
	if (__high) {    														\
		__upper = __high % (__base);										\
		__high = __high / (__base);											\
	}																		\
 																			\
	asm("divl %2" : "=a" (__low), "=d" (__mod)								\
			: "rm" (__base), "" (__low), "1" (__upper));					\
	asm("" : "=A" (n) : "a" (__low), "d" (__high));							\
	__mod;																	\
})

#endif
