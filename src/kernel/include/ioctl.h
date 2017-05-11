/*
 *
 * Elarix
 * src/kernel/include/ioctl.h
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

#ifndef _IOCTL_H
#define _IOCTL_H

/* TODO Define these in the modules themselves */

/* Timer */
#define IO_UPTIME	0x01

/* RTC */
#define IO_TIME		0x02

/* Keyboard */
#define IO_KEYEV	0x03

/* Block devices */
#define IO_EJECT	0x04

#endif
