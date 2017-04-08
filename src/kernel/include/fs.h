/*
 *
 * Elarix
 * src/kernel/include/fs.h
 *
 * Copyright (C) 2017 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

/* Superblock flags */
#define SF_KEEP		1	/* Don't remove from cache */

#if 0
/* Inode flags */
#define IF_RO		1	/* Read only */
#define IF_AO		2	/* Append only */
#define IF_IMMUTABLE	4	/* Immutable */
#define IF_NOATIME	8	/* Do not update access time */
#define IF_SYNC	16	/* Synchronous updates */
#endif

/* Inode mode */
#define IM_FTM		0170000
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
	u8	flags;		/* Superblock flags */

	/* u64	fb; */		/* First block */
	/* u16	bsize; */		/* Block size */
	/* u64	size_max; */	/* Max. file size */

	struct dirent *root;	/* Root dirent */
	struct list_head il;	/* Inodes */

	struct sb_ops *op;
};

struct inode {
	struct list_head l;

	ino_t	inum;			/* Inode number */
	/* u8	flags; */			/* Inode flags */
	mode_t	mode;			/* Inode mode */

	int	refs;			/* Reference count */
	link_t	links;			/* Link count */

	dev_t	dev;			/* Device ID (only for devices) */
	uid_t	uid;			/* User ID */
	gid_t	gid;			/* Group ID */

	time_t	atime;			/* Access time */
	time_t	ctime;			/* Change time */
	time_t	mtime;			/* Modification time */

	off_t	size;			/* File size in bytes */

	struct superblock	*sp;	/* Associated superblock */
	struct list_head	del;	/* List of children (only for dirs.) */

	struct inode_ops	*op;
	struct file_ops		*fop;
};

struct dirent {
	struct list_head l;

	char	name[NAME_MAX + 1];	/* File name */

	int	refs;			/* Reference count */

	struct inode	*ip;		/* Associated inode pointer */
	struct dirent	*dp;		/* Associated directory pointer */
};

struct usr_dirent {
	ino_t	inum;			/* Inode number */

	char	name[NAME_MAX + 1];	/* File name */
};

struct file {
	mode_t	mode;			/* File mode */
	off_t	off;			/* Current offset */

	int	refs;			/* Reference count */

	struct dirent	*dep;		/* Associated directory entry pointer */
};

struct sb_ops {
	/* Allocate a memory inode: sp */
	struct inode *(*inode_alloc) (struct superblock *);
	/* Deallocate a memory inode: ip */
	void (*inode_dealloc) (struct inode *);
	/* Write an inode to disk: ip */
	int (*inode_write) (struct inode *);
	/* Delete an inode from disk: ip */
	void (*inode_delete) (struct inode *);
	/* TODO (sync) */
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
	int (*move) (struct inode *, struct dirent *, /* XXX Eq to rename on l */
			struct inode *, struct dirent *);
	/* TODO (perm), (setattr / getattr), (readlink) */
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
	int (*read) (struct file *, char *, off_t, size_t);
	/* Write n bytes from buf into fp at off: fp, buf, off, n */
	int (*write) (struct file *, const char *, off_t, size_t);
	/* Read next directory: fp, TODO */
	int (*readdir) (struct file *, struct usr_dirent *);
	/* I/O control: fp, cmd, arg */
	int (*ioctl) (struct file *, unsigned int, unsigned long);
	/* TODO (sync / fsync) */
};

extern struct superblock *root_sb;

struct superblock *sb_alloc(struct fs_driver *driver);

struct inode *inode_alloc(struct superblock *sp);
struct inode *inode_get(struct superblock *sp, ino_t inum);
void inode_put(struct inode *ip);

struct dirent *dirent_alloc(struct dirent *dp, const char *name);
struct dirent *dirent_alloc_root(struct inode *ip);

struct dirent *dirent_get(const char *path);
struct usr_dirent *usr_dirent_get(struct file *fp);
void dirent_put(struct dirent *dep);

struct file *file_alloc(struct dirent *dep);
struct file *file_get(int fd);
void file_put(struct file *fp);

int fd_alloc(struct file *fp);
void fd_dealloc(int fd);

int sv_mkdir(const char *path, mode_t mode);

int fs_reg(struct fs_driver *driver);
void fs_unreg(struct fs_driver *driver);

/* XXX TMP XXX */
int sys_chdir(const char *path);
int sys_cwdir(char *path);
int sys_write(int fd, const char *buf, size_t n);
int sys_open(const char *path, int flags, mode_t mode);
int sys_close(int fd);
int sys_readdir(int fd, struct usr_dirent *udep);
int sys_mkdir(const char *path, mode_t mode);
/* XXX END TMP XXX */

/* END fs.h */


/* BEGIN dev.h */

#define MAJOR_ZERO	0
#define MAJOR_MEM	1
#define MAJOR_CON	2
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

int dev_init(dev_t dev);

/* END dev.h */

/* BEGIN mount.h */

struct mountp {
	struct mountp	*pp;	/* Parent fs */

	struct superblock *sp;	/* Superblock */

	char *name;		/* Device path or name */
};

struct mountp *sv_mount(struct fs_driver *driver, const char *name);

/* END mount.h */

#endif
