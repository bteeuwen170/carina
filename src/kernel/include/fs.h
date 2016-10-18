/*
 *
 * Carina
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

/* Inode and dirent types */
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
///* The sticky bit is pretty useless, so it's ignored in Carina */

struct block_dev {
	dev_t	dev;	/* Device identifier */

//	u64	fb;	/* First block */
//	u16	bsize;	/* Block size */

	ino_t	inum;	/* First inode */
	ino_t	inodes;	/* Number of inodes */

	void	**data;	/* Data pointer for virual devices (optional) */

	struct block_dev_ops	*op;

	struct block_dev	*next;
};

struct mountp {
	dev_t	dev;

	char	path[PATH_MAX];
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

//	u32	bsize;	/* Block size */	
	off_t	size;	/* File size in bytes */

	struct block_dev	*dev;	/* Block device structure */
};

struct dirent {
	ino_t	inum;
	u8	type;

	char	name[NAME_MAX + 1];
} __attribute__ ((packed));

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

//typedef struct DIR {
//	struct dirent	*cur;
//	char		name[NAME_MAX + 1];
//} DIR;

struct file_ops {
	///* Read n bytes at off from fp into buf: fp, buf, off, n */
	//int (*read) (struct file *, void *, off_t, size_t);
	///* Read next directory: fp,  */
	//int (*readdir) (struct file *, void *);
	///* Write n bytes from buf into fp at off: fp, buf, off, n */
	//int (*write) (struct file *, const void *, off_t, size_t);
	///* Create a new file object: ip, &fp */
	//int (*open) (struct inode *, struct file *);
	///* Delete a file object: ip, fp */
	//int (*close) (struct inode *, struct file *);
	//TODO (ioctl), (sync / fsync)
};

struct block_dev_ops {

//struct sb_ops {
	/* Allocate a new inode object: sb, &ip */
	struct inode *(*alloc_inode) (struct block_dev *);
	/* Write an inode: ip */
	void (*write_inode) (struct inode *);
	/* Delete an inode: ip */
	//TODO (delete_inode)
	/* Delete an inode object: ip */ //XXX Do we need this?
	//TODO (drop_inode)
	/* Unmount a block device: dev */
	//TODO (put_super)
//};

//struct inode_ops {
	/* Create a file: dp, dep, mode */
	int (*create) (struct inode *, struct dirent *, mode_t);
	/* Create a hard link: dep, dp, name */
	int (*link) (struct dirent *, struct inode *, const char *);
	/* Create a symbolic link: dp, dep, name */
	int (*symlink) (struct inode *, struct dirent *, const char *);
	///* Delete a link: dp, dep */
	//int (*unlink) (struct inode *, struct dirent *);
	///* Create a directory: dp, dep, mode */
	//int (*mkdir) (struct inode *, struct dirent *, mode_t);
	///* Delete a directory: dp, dep */
	//int (*rmdir) (struct inode *, struct dirent *);
	///* Move a dirent: odp, odep, dp, dep */
	//int (*move) (struct inode *, struct dirent *,
	//		struct inode *, struct dirent *);
	//TODO (mknod)
//};

	struct file_ops *dfop;


	/* FIXME Crap */
	//int (*create) (struct inode *, const char *, int, int, struct inode **);

	//int (*iread) (struct inode *);
	//int (*iwrite) (struct inode *);

	//int (*read) (struct inode *, char *, int);
	//int (*mkdir) (struct inode *, char *, void *, );
	//int (*readdir) (struct inode *, void *);

	/* Copy an inode to memory: dev, inum */
	//int (*open) (struct block_dev *, ino_t);
	/* Read from an inode: ip, data, off, n */
	//int (*read) (struct inode *, void *, u64, u64);
	/* Write to an inode: ip, data, off, n */
	//int (*write) (struct inode *, const void *, u64, u64);
};

void dev_reg(struct block_dev *dp);

#endif
