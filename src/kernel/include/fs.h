/*
 *
 * Elara
 * src/kernel/include/fs.h
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

#include <limits.h>
#include <list.h>

///* Inode and dirent types */
//#define IT_FILE		1	/* Regular */
//#define IT_LINK		2	/* Symbolic link */
//#define IT_DIR		4	/* Directory */
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
///* The sticky bit is pretty useless, so it's ignored in Elara */

/* BEGIN mount.h */

struct mountp {
	struct dirent *dp;	/* Directory entry */
	struct block_dev *sb;	/* Superblock */
};

/* END mount.h */

/* BEGIN path.h */

struct path {
	struct mountp *mnt;	/* Root mount point */
	struct dirent *dp;	/* Path */
};

/* END path.h */

/* BEGIN fs.h */

struct superblock {
//	dev_t	dev;	/* Device identifier */

//	u64	fb;	/* First block */
//	u16	bsize;	/* Block size */

	ino_t	inum;	/* First inode */
	ino_t	inodes;	/* Number of inodes */

	void	**data;	/* Data pointer for virual devices (optional) */
	void	*info;

	struct block_dev_ops	*op;

	struct list_head	l;
};

struct inode {
	ino_t	inum;	/* Inode number */
	u8	type;	/* Inode type */
//	u8	flags;	/* Inode flags */
	mode_t	mode;	/* Inode mode */

	u32	refs;	/* References count */
	link_t	links;	/* Link count */

	uid_t	uid;	/* User ID */
	gid_t	gid;	/* Group ID */

	time_t	atime;	/* Access time */
	time_t	ctime;	/* Change time */
	time_t	mtime;	/* Modification time */

	off_t	size;	/* File size in bytes */

	struct superblock	*sb;	/* Associated superblock */

	struct inode_ops	*op;	/* Inode operations */
};

struct dirent {
	struct inode	*ip;	/* Associated inode pointer */
	u8		type;	/* Dirent type */
	u32		refs;	/* References count */

	char	name[NAME_MAX + 1];
};

struct file {
	struct path	path;	/* File path */
	mode_t		mode;	/* File access mode */
	u8		flags;	/* File flags */

	struct file_ops	*op;	/* File operations */
};

struct sb_ops {
	/* Allocate a memory inode: sb */
	struct inode *(*alloc_inode) (struct block_dev *);
	/* Deallocate a memory inode: ip */
	void *(*dealloc_inode) (struct inode *);
	/* Write an inode to disk: ip */
	int (*write_inode) (struct inode *);
	/* Delete an inode from disk: ip */
	int (*delete_inode) (struct inode *);
	//TODO (sync)
};

struct inode_ops {
	/* Create a file: dp, dep, mode */
	int (*create) (struct inode *, struct dirent *, mode_t);
	/* Create a hard link: dep, dp, name */
	int (*link) (struct dirent *, struct inode *, struct dirent *);
	/* Create a symbolic link: dp, dep, name */
	int (*symlink) (struct inode *, struct dirent *, const char *);
	/* Delete a link: dp, dep */
	int (*rmlink) (struct inode *, struct dirent *);
	/* Create a directory: dp, dep, mode */
	int (*mkdir) (struct inode *, struct dirent *, mode_t);
	/* Delete a directory: dp, dep */
	int (*rmdir) (struct inode *, struct dirent *);
	/* Move a dirent: odp, odep, dp, dep */
	int (*move) (struct inode *, struct dirent *, //XXX Eq to rename on l
			struct inode *, struct dirent *);
	//TODO (mknod), (perm), (setattr / getattr), (readlink)
};

struct dirent_ops {
	/* Close the directory entry: dp */
	int (*close) (struct dirent *);
};

struct file_ops {
	/* Read n bytes at off from fp into buf: fp, buf, off, n */
	int (*read) (struct file *, void *, off_t, size_t);
	/* Read next directory: fp, TODO */
	int (*readdir) (struct file *, void *);
	/* Write n bytes from buf into fp at off: fp, buf, off, n */
	int (*write) (struct file *, const void *, off_t, size_t);
	/* Create a new file object: ip, &fp */
	int (*open) (struct inode *, struct file *);
	/* Delete a file object: ip, fp */
	int (*close) (struct inode *, struct file *); //XXX Eq to release on l
	//TODO (ioctl), (sync / fsync)
};

/* END fs.h */

void dev_reg(struct block_dev *dp);

#endif
