/*
 *
 * Elarix
 * src/kernel/include/fs.h
 *
 * Copyright (C) 2016 - 2017 Bastiaan Teeuwen <bastiaan@mkcl.nl>
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

#include <stdarg.h>

/* Mount flags */
#define M_KEEP		02	/* Keep inodes and dirents cached */
#define M_RO		01	/* Mount read-only */

/* Inode modes */
#define I_REG		0100000	/* Regular file */
#define I_DIR		0040000	/* Directory */
#define I_DEV		0020000	/* Device */
#define I_LINK		0010000	/* Symbolic link */

#define I_SUID		0002000	/* Set UID on execution */
#define I_SGID		0001000	/* Set GID on execution */

#define I_UM		0000700
#define I_UR		0000400	/* Owner read */
#define I_UW		0000200	/* Owner write */
#define I_UE		0000100	/* Owner execute */

#define I_GM		0000070
#define I_GR		0000040	/* Group read */
#define I_GW		0000020	/* Group write */
#define I_GE		0000010	/* Group execute */

#define I_OM		0000007
#define I_OR		0000004	/* Other read */
#define I_OW		0000002	/* Other write */
#define I_OE		0000001	/* Other execute */

/* File modes */
#define F_DIR		04	/* File is a directory */
#define F_CREATE	02	/* Create file if doesn't exist */
#define F_RO		01	/* Read-only access */

struct fs_driver {
	struct list_head l;

	const char	*name;	/* File system name */
	u8		flags;	/* Mount flags */

	struct fs_ops *op;	/* File system operations */
};

struct superblock {
	struct list_head l;

	dev_t	dev;			/* Device identifier */
	char	name[NAME_MAX + 1];	/* Device name */
	u8	flags;			/* Mount flags */

	u64	blocks;			/* Block count */
	u16	block_size;		/* Block size */

	struct list_head	bl;	/* Block cache */
	struct list_head	il;	/* Inode cache */

	struct inode		*root;	/* Root inode */
	struct dirent		*pdep;	/* Parent directory entry pointer */

	struct fs_driver	*fsdp;	/* File system driver */
};

struct inode {
	struct list_head l;
	int refs;

	ino_t	inum;			/* Inode number */
	mode_t	mode;			/* Inode mode */
	link_t	links;			/* Link count */

	dev_t	rdev;			/* Device ID */
	uid_t	uid;			/* User ID */
	gid_t	gid;			/* Group ID */

	time_t	atime;			/* Access time */
	time_t	ctime;			/* Change time */
	time_t	mtime;			/* Modification time */

	off_t	block;			/* Block number */
	off_t	size;			/* File size */

	struct superblock	*sp;	/* Associated superblock */
	struct list_head	del;	/* List of children */

	struct file_ops *op;		/* File operations */
};

struct dirent {
	struct list_head l;
	int refs;

	ino_t	inum;			/* Inode number */
	char	name[NAME_MAX + 1];	/* File name */

	/* FIXME This feel wrong */
	struct superblock	*sp;	/* Associated superblock */
	struct dirent		*pdep;	/* Parent dirent pointer */
};

struct file {
	off_t	off;		/* Current offset */
	mode_t	mode;		/* File mode */

	struct dirent *dep;	/* Associated directory entry pointer */
	struct inode *ip;	/* Associated inode pointer */
};

struct fs_ops {
	/* Read the superblock: sp */
	int (*sb_get) (struct superblock *);
	/* Read the superblock: sp */
	int (*sb_put) (struct superblock *);

	/* Get an inode: ip */
	int (*alloc) (struct inode *);
	/* Get a directory entry: dp, name, &dep */
	int (*lookup) (struct inode *, const char *, struct dirent **);

	/* Create a regular file: dp, dep, mode */
	int (*mkreg) (struct inode *, struct dirent *, mode_t);
	/* Create a directory: dp, dep, mode */
	int (*mkdir) (struct inode *, struct dirent *, mode_t);
	/* Create a special file: dp, dep, mode, dev */
	int (*mkdev) (struct inode *, struct dirent *, mode_t, dev_t);
	/* Create a symbolic link: dp, dep, sdep */
	int (*mklnk) (struct inode *, struct dirent *, struct dirent *);
	/* Delete a link: dp, dep */
	int (*rmlnk) (struct inode *, struct dirent *);
	/* Create a hard link: dp, dep, ip */
	int (*link) (struct inode *, struct dirent *, struct inode *);
	/* Move a dirent: odp, odep, dp, dep */
	int (*move) (struct inode *, struct dirent *,
			struct inode *, struct dirent *);
};

struct file_ops {
	/* Read n bytes at off from fp into buf: fp, buf, off, n */
	int (*read) (struct file *, char *, off_t, size_t);
	/* Write n bytes from buf into fp at off: fp, buf, off, n */
	int (*write) (struct file *, const char *, off_t, size_t);
	/* Read next directory: fp, _name */
	int (*readdir) (struct file *, char *);
	/* I/O control: fp, cmd, args */
	int (*ioctl) (struct file *, unsigned int, va_list);
	/* TODO (sync / fsync) */
};

extern struct dirent *fs_root;

int sb_get(struct fs_driver *fsdp, dev_t dev, u8 flags, struct superblock **sp);
int sb_put(struct dirent *dep);
int sb_lookup(struct inode *dp, const char *name, struct superblock **sp);

int inode_get(struct superblock *sp, ino_t inum, struct inode **ip);
void inode_put(struct inode *ip);
int inode_dirisempty(struct inode *ip); /* XXX Keep this? */

int dir_get(const char *path, struct dirent **dep);
void dir_put(struct dirent *dep);
int dir_lookup(struct inode *dp, const char *name, struct dirent **dep);
int dir_basepath(char *path);
int dir_basename(char *path);

int file_open(const char *path, mode_t mode, struct file **fp);
void file_close(struct file *fp);
int file_read(struct file *fp, char *buf, size_t n);
int file_write(struct file *fp, const char *buf, size_t n);
int file_readdir(struct file *fp, char *_name);
int file_ioctl(struct file *fp, unsigned int cmd, ...);
/* int file_stat(struct file *fp, struct stat *sp); */

/* int fs_mkreg(const char *path, mode_t mode); */
int fs_mkdir(const char *path, mode_t mode);
/* int fs_mkdev(const char *path, mode_t mode, dev_t dev); */
/* int fs_mklnk(const char *src_path, const char *path); */
/* int fs_rmlnk(const char *path); */
/* int fs_link(const char *src_path, const char *path); */
/* int fs_move(const char *src_path, const char *path); */

int fs_chdir(const char *path);
int fs_cwdir(char *_path);
int fs_mount(dev_t dev, const char *path, const char *fs, u8 flags);
int fs_unmount(const char *path);

int fs_reg(struct fs_driver *fsdp);
void fs_unreg(struct fs_driver *fsdp);

void fs_init(void);

/* TODO Find a proper place for these non-modular file systems */
int memfs_init(void);
void memfs_exit(void);

int devfs_init(void);
void devfs_exit(void);

#endif
