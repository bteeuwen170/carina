/*
 *
 * Elarix
 * src/kernel/lib/math.h
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

#ifndef _MATH_H
#define _MATH_H

typedef struct {
	int quot;	/* Quotient */
	int rem;	/* Remainder */
}

//double sin(double sin);
//double cos(double cos);
//double tan(double tan);

//double log(double val);

int abs(int val);
int div(int num, int denom);

//double ceil(double val);
//double floor(double val);

#endif
