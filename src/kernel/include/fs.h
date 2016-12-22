/*
 *
 * Elarix
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

/* BEGIN fs.h */

#include <limits.h>
#include <list.h>

///* Inode flags */
//#define IF_RO		1	/* Read only */
//#define IF_AO		2	/* Append only */
//#define IF_IMMUTABLE	4	/* Immutable */
//#define IF_NOATIME	8	/* Do not update access time */
//#define IF_SYNC	16	/* Synchronous updates */

/* Inode permissions */
#define IM_FTM		0170000
//#define IM_SOCK	0140000	/* Socket */
#define IM_LNK		0120000	/* Symbolic link */
#define IM_REG		0100000	/* Regular file */
#define IM_BLK		0060000	/* Block device */
#define IM_DIR		0040000	/* Directory */
#define IM_CHR		0020000	/* Character device */
#define IM_FIFO		0010000	/* Pipe */

#define IM_SUID		0004000	/* Set UID on execution */
#define IM_SGID		0002000	/* Set GID on execution */

#define IM_UM		0000700
#define IM_UR		0000400	/* Owner read */
#define IM_UW		0000200	/* Owner write */
#define IM_UE		0000100	/* Owner execute */

#define IM_GM		0000070
#define IM_GR		0000040	/* Group read */
#define IM_GW		0000020	/* Group write */
#define IM_GE		0000010	/* Group execute */

#define IM_OM		0000007
#define IM_OR		0000004	/* Other read */
#define IM_OW		0000002	/* Other write */
#define IM_OE		0000001	/* Other execute */

struct fs_driver {
	struct list_head l;

	const char *name;

	/* Read the superblock: sp, data */
	struct superblock *(*read_sb) (struct superblock *);
};

struct superblock {
	struct list_head l;

	dev_t	dev;		/* Device identifier */

//	u64	fb;		/* First block */
//	u16	bsize;		/* Block size */
//	u64	size_max;	/* Max. file size */

	struct list_head il;	/* Inodes */
//	ino_t	inum;		/* First inode */
	ino_t	inodes;		/* Number of inodes */

	struct dirent *root;	/* Root dirent */

	struct sb_ops *op;
};

struct inode {
	struct list_head l;

	dev_t	dev;		/* Device ID */
	ino_t	inum;		/* Inode number */
//	u8	flags;		/* Inode flags */
	mode_t	mode;		/* Inode mode */

	int	refs;		/* References count */
	link_t	links;		/* Link count */

	uid_t	uid;		/* User ID */
	gid_t	gid;		/* Group ID */

	time_t	atime;		/* Access time */
	time_t	ctime;		/* Change time */
	time_t	mtime;		/* Modification time */

	off_t	size;		/* File size in bytes */

	struct superblock *sp;	/* Associated superblock */

	struct inode_ops  *op;
	struct file_ops  *fop;
};

struct dirent {
	struct list_head l;

	struct inode		*ip;	/* Associated inode pointer */
	struct dirent		*dp;	/* Associated directory pointer */
	struct list_head	del;	/* List of children */

	int	refs;			/* References count */

	char	name[NAME_MAX + 1];
};

struct file {
	struct list_head l;

	struct mountp		*mp;	/* Associated mountpoint */
	struct dirent		*dp;	/* Associated directory pointer */

	int	refs;
	mode_t	mode;	/* File access mode */

	off_t	off;

	struct file_ops	*op;
};

struct sb_ops {
	/* Allocate a memory inode: sp */
	struct inode *(*alloc_inode) (struct superblock *);
	/* Deallocate a memory inode: ip */
	void (*dealloc_inode) (struct inode *);
	/* Write an inode to disk: ip */
	int (*write_inode) (struct inode *);
	/* Delete an inode from disk: ip */
	int (*delete_inode) (struct inode *);
	//TODO (sync)
};

struct inode_ops {
	/* Create a file: dp, dep, mode */
	int (*create) (struct inode *, struct dirent *, mode_t);
	/* Create a hard link: dp, dep, name */
	int (*link) (struct inode *, struct dirent *, struct dirent *);
	/* Create a symbolic link: dp, dep, name */
	int (*symlink) (struct inode *, struct dirent *, const char *);
	/* Delete a link: dp, dep */
	int (*rmlink) (struct inode *, struct dirent *);
	/* Create a directory: dp, dep, mode */
	int (*mkdir) (struct inode *, struct dirent *, mode_t);
	/* Delete a directory: dp, dep */
	int (*rmdir) (struct inode *, struct dirent *);
	/* Create a special file: dp, dep, mode, dev */
	int (*mknod) (struct inode *, struct dirent *, mode_t, dev_t);
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
	/* Create a new file object: ip, &fp */
	int (*open) (struct inode *, struct file *);
	/* Delete a file object: ip, fp */
	int (*close) (struct inode *, struct file *);
	/* Read n bytes at off from fp into buf: fp, buf, off, n */
	int (*read) (struct file *, void *, off_t, size_t);
	/* Write n bytes from buf into fp at off: fp, buf, off, n */
	int (*write) (struct file *, const void *, off_t, size_t);
	/* Read next directory: fp, TODO */
	int (*readdir) (struct file *, void *);
	//TODO (ioctl), (sync / fsync)
};

struct superblock *sb_alloc(struct fs_driver *driver);

struct inode *inode_alloc(struct superblock *sp);
void inode_dealloc(struct inode *ip);

struct dirent *dirent_alloc(struct dirent *dp, const char *name);
struct dirent *dirent_alloc_root(struct inode *rp);
void *dirent_get(struct file *fp);

int fs_reg(struct fs_driver *driver);
void fs_unreg(struct fs_driver *driver);

/* END fs.h */


/* BEGIN dev.h */

#define MAJOR_ZERO	0
#define MAJOR_MEM	1
#define MAJOR_PTY	2
#define MAJOR_KBD	3
#define MAJOR_MOUSE	4
#define MAJOR_DISK	5
#define MAJOR_OPTICAL	6
#define MAJOR_SOUND	7
#define MAJOR_MISC	255

struct device {
	const char	*name;
	struct file_ops	*op;
};

int dev_reg(u8 major, const char *name, struct file_ops *op);
void dev_unreg(u8 major);

/* END dev.h */

/* BEGIN mount.h */

struct mountp {
	struct mountp	*pp;	/* Parent fs */
	struct dirent	*dp;	/* Directory entry */

	struct superblock *sp;	/* Superblock */

	char *name;		/* Device path or name */
};

struct mountp *sv_mount(struct fs_driver *driver, const char *name);

/* END mount.h */

#endif
