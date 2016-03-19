/*
 *
 * Carina
 * Header for CFS
 *
 * Copyright (C) 2015 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#ifndef __CFS_H_
#define __CFS_H_

#include <stdlib.h>

#define FILENAME_LENGTH	38

/* File attributes */
#define ATTR_DIR		0b00001
#define ATTR_SYS		0b00010
#define ATTR_WRITE		0b00100
#define ATTR_EXEC		0b01000
#define ATTR_HIDE		0b10000

struct superblock {
	u16			magic;				/* Magic Number */
	u8			block_size;			/* Sectors per block */ 
	u16			base_offset;	 	/* Base offset */
	u64			sectors;		 	/* Number of sectors */
	u8			sector_size;	 	/* Bytes per sector */ //TODO Remove [2]
	u64			fat_length;		 	/* Sectors per FAT */
	u16			backup_sb;			/* Backup superblock */
	u8			uuid[16]; 			/* Volume UUID */
	char		label[16];			/* Volume label */
} __attribute__ ((packed));

/* Directory entry */
struct dir_entry {
	char		name[38];			/* File name */
	u8			attr;				/* File attributes */
	u64			ctime;				/* Creation time */
	u64			atime;				/* Last access time */
	u64			mtime;				/* Last modified time */
	u64			block;				/* First block */
	u64			size;				/* File size */
} __attribute__ ((packed));

#endif
