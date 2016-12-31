/*
 *
 * Elarix
 * src/kernel/dev/char/pcspk.h
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

#ifndef _PCSPK_H
#define _PCSPK_H

void pcspk_play(const u16 freq);
void pcspk_stop(void);

void pcspk_fj(void); /* Frere Jacques */
void pcspk_mi(void); /* The Secret Of Monkey Island intro */
void pcspk_hc(void); /* Eagles - Hotel California */
void pcspk_acri(void); /* Bob Acri - Sleep Away */

#endif
