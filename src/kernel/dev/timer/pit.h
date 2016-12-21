/*
 *
 * Elara
 * src/kernel/dev/timer/pit.h
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

#ifndef _PIT_H
#define _PIT_H

#include <sys/time.h>

#define PIT_RATE	0b00110110
#define PIT_SPKR	0b10110110

#define PIT_FREQ	0x1234DE

#define PIT_CMD		0x43
#define PIT_CH0_IO	0x40
#define PIT_CH2_IO	0x42

time_t uptime(void);

void sleep(const u64 delay);

int timer_init(void);

#endif
