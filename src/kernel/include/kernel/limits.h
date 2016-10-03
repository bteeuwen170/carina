/*
 *
 * Carina
 * src/kernel/include/kernel/limits.h
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

/* TODO Relocate in seperate files and delete this */

#ifndef _KERNEL_LIMITS_H
#define _KERNEL_LIMITS_H


//TODO Define max and min of data types

#define BLOCK_SIZE	1024	/* Block size */
#define SECTOR_SIZE	512	/* Sector size */

#define FILES_MAX	8	/* Maximum number of open files at once */
#define PATH_MAX	255	/* Maximum length of a pathname */

#define LINKS_MAX	127	/* Maximum number of links for a file */

#endif
