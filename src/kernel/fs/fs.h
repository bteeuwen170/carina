
/*
 *
 * Carina
 * src/kernel/fs/fs.h
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

#ifndef _FS_H
#define _FS_H

enum ftype {
	FTYPE_FILE,
	FTYPE_LINK,
	FTYPE_DIR,
	FTYPE_BLOCK,
	FTYPE_CHAR,
	FTYPE_PIPE
};

enum fmode {
	FMODE_UR,	/* Owner read */
	FMODE_UW,	/* Owner write */
	FMODE_UE,	/* Owner execute */
	FMODE_GR,	/* Group read */
	FMODE_GW,	/* Group write */
	FMODE_GE,	/* Group execute */
	FMODE_OR,	/* Other read */
	FMODE_OW,	/* Other write */
	FMODE_OE,	/* Other execute */
	FMODE_SUID,	/* Set UID on execution */
	FMODE_SGID	/* Set GID on execution */
	/* The sticky bit is pretty useless, so it's not supported in Carina */
};

/* TODO */
struct inode {
	u32	mode;
	u32	uid;
	u32	gid;
	u32	size;
	u32	blocks;
	u32	references;
	u32	flags;
	u32	size;
};

#endif
