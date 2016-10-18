/*
 *
 * Carina
 * src/kernel/include/reboot.h
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

#ifndef _REBOOT_H
#define _REBOOT_H

#define REBOOT_HALT	0x01	/* Halt the system */
#define REBOOT_POWEROFF	0x02	/* Power the system off (if supported) */
#define REBOOT_RESTART	0x03	/* Restart the system */
#define REBOOT_PANIC	0x04	/* Force a kernel panic */
#define REBOOT_KALL	0x05	/* Kill all processes */

void reboot(void);

#endif
