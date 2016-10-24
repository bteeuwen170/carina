/*
 *
 * Elara
 * src/kernel/include/limits.h
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

#ifndef _LIMITS_H
#define _LIMITS_H

/* FIXME Not here */
//#define BLOCK_SIZE	1024	/* Block size */
//#define SECTOR_SIZE	512	/* Sector size */
//#define CPUS_MAX	8	/* Max. number of CPUs */

#define CHILD_MAX	512	/* Max. number of processes */
//#define LINK_MAX	32	/* Max. number of links for a file */
#define NAME_MAX	255	/* Max. length of a file name */
//#define NGROUPS_MAX	8	/* Max. number of GIDs per process */
#define OPEN_MAX	32	/* Max. open files */
#define PATH_MAX	4096	/* Max. length of a path name */
//#define INODES_MAX	64	/* Max. inodes in memory */

#endif
