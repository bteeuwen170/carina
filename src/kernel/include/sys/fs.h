/*
 *
 * Carina
 * src/kernel/include/kernel/fs.h
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

#ifndef _SYS_FS_H
#define _SYS_FS_H

#include <kernel/limits.h>

/* Inode types */
#define IT_FILE		1	/* Regular */
#define IT_LINK		2	/* Symbolic link */
#define IT_DIR		4	/* Directory */
//#define IT_BLOCK	8	/* Block device */
//#define IT_CHAR		16	/* Character device */
//#define IT_PIPE		32	/* Pipe */

///* Inode flags */
//#define IF_RO		1	/* Read only */
//#define IF_AO		2	/* Append only */
//#define IF_IMMUTABLE	4	/* Immutable */
//#define IF_NOATIME	8	/* Do not update access time */
//#define IF_SYNC		16	/* Synchronous updates */
//
///* Inode permissions */
//#define IM_UR		1	/* Owner read */
//#define IM_UW		2	/* Owner write */
//#define IM_UE		8	/* Owner execute */
//#define IM_GR		16	/* Group read */
//#define IM_GW		32	/* Group write */
//#define IM_GE		64	/* Group execute */
//#define IM_OR		128	/* Other read */
//#define IM_OW		256	/* Other write */
//#define IM_OE		512	/* Other execute */
//#define IM_SUID		1024	/* Set UID on execution */
//#define IM_SGID		2048	/* Set GID on execution */
///* The sticky bit is pretty useless, so it's not supported in Carina */

struct block_dev {
	u32	dev;	/* Device identifier */

//	u64	fb;	/* First block */
//	u16	bsize;	/* Block size */

	u64	inum;	/* First inode */
	u64	inodes;	/* Number of inodes */

	void	**data;	/* Data pointer for virual devices (optional) */

	struct block_dev_ops	*op;

	struct block_dev	*next;
};

struct mountp {
	u32	dev;

	char	path[PATH_MAX];
};

struct inode {
	struct block_dev	*dev;	/* Block device structure */
	u64			inum;	/* Inode number */

	u8	type;	/* Inode type */
//	u8	flags;	/* Inode flags */
//	u16	perms;	/* Inode mode */

	u32	refs;	/* References count */
	u32	links;	/* Link count */

	uid_t	uid;	/* User ID */
	gid_t	gid;	/* Group ID */

	time_t	atime;	/* Access time */
	time_t	ctime;	/* Change time */
	time_t	mtime;	/* Modification time */

//	u32	bsize;	/* Block size */	
	u64	size;	/* File size in bytes */
};

struct dirent {
	u64	inum;
	char	name[NAME_MAX];
};

struct block_dev_ops {
	/* FIXME Crap */
	//int (*create) (struct inode *, const char *, int, int, struct inode **);

	//int (*iread) (struct inode *);
	//int (*iwrite) (struct inode *);

	//int (*read) (struct inode *, char *, int);
	//int (*mkdir) (struct inode *, char *, void *, );
	//int (*readdir) (struct inode *, void *);


	/* Create new inode: sb, &ip */
	struct inode *(*alloc) (struct block_dev *);
	/* Read from an inode: ip, data, off, n */
	int (*read) (struct inode *, void *, u64, u64);
	/* Write to inode: ip, data, off, n */
	int (*write) (struct inode *, const void *, u64, u64);
};

//enum file_types {
//	FT_FILE		= 1,	/* Regular */
//	FT_LINK		= 2,	/* Symbolic link */
//	FT_DIR		= 4,	/* Directory */
//	FT_BLOCK	= 8,	/* Block device */
//	FT_CHAR		= 16,	/* Character device */
//	FT_PIPE		= 32	/* Pipe */
//};
//
//struct file {
//	int		fd;	/* File descriptor */
//	u8		type;	/* File type */
//	u32		refs;	/* References count XXX */
//	u8		flags;	/* File flags */
//
//	union {
//		struct inode	*inode;
//		struct pipe	*pipe;
//	} typep;
//};

void dev_reg(struct block_dev *dp);

int dir_write(struct inode *dp, u64 inum, char *name);

#endif
