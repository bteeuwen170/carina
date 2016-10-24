/*
 *
 * Elara
 * src/kernel/include/fcntl.h
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

#ifndef _FCNTL_H
#define _FCNTL_H

/*
 * File creation flags
 */
#define O_CREAT		0x200	/* Create file if doesn't exist */

/*
 * File access modes (mode_t)
 */
#define O_ACCMODE	0x03
#define O_RDONLY	0x00	/* Read only */
#define O_WRONLY	0x01	/* Write only */
#define O_RDWD		0x02	/* Read and write */

#endif
