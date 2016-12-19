/*
 *
 * Elara
 * src/kernel/fs/ramfs/ramfs.c
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

#include <errno.h>
#include <fs.h>
#include <fs/ramfs.h>
#include <limits.h>
#include <kernel.h>
#include <sys/types.h> /* XXX TEMP for syntax highlighting */

#include <stdlib.h>
#include <string.h>

#if 0
/* Block size */
//#define RAMFS_BS	0

/* Inodes per block */
//#define RAMFS_IPB	(RAMFS_BS / sizeof(struct ramfs_inode))

/* Inode */
//#define INODE(inum, sb)	((inum) / RAMFS_IPB + sb.fb)
//
#define RAMFS_NAME_MAX	(NAME_MAX)

struct ramfs_dirent {
	ino_t	inum;
	u8	type;

	char	name[RAMFS_NAME_MAX + 1];
};

struct ramfs_inode {
	u8	type;
	mode_t	mode;

	link_t	links;

	uid_t	uid;
	gid_t	gid;

	time_t	atime;
	time_t	ctime;
	time_t	mtime;

	off_t	size;

	void	*data;
};

/*
 * Allocate a new inode
 */
static struct inode *ramfs_alloc_inode(struct superblock *sp)
{
	ino_t i;

	/* FIXME Skips lot's of empty inodes for some reason... */
	for (i = 1; i < sp->inodes; i++) {
		//kprintf("crap: %u\n",
		//		((struct ramfs_inode *) sp->data[i])->type);
		if (((struct ramfs_inode *) sp->data[i]) != 0)
			continue;

		sp->data[i] = kmalloc(sizeof(struct ramfs_inode));

		struct inode *ip;

		ip = kmalloc(sizeof(struct inode));

		if (!ip)
			return NULL;

		ip->sp = sp;
		ip->inum = i;

		ip->type = ((struct ramfs_inode *) sp->data[i])->type;
		ip->mode = 0;

		ip->refs = 1;
		ip->links = 0;

		ip->uid = ((struct ramfs_inode *) sp->data[i])->uid;
		ip->gid = ((struct ramfs_inode *) sp->data[i])->gid;

		ip->atime = ((struct ramfs_inode *) sp->data[i])->atime;
		ip->ctime = ((struct ramfs_inode *) sp->data[i])->ctime;
		ip->mtime = ((struct ramfs_inode *) sp->data[i])->mtime;

		ip->size = ((struct ramfs_inode *) sp->data[i])->size;

		return ip;
	}

	return NULL;
}

/*
 * Write an inode
 */
static void ramfs_write_inode(struct inode *ip)
{
	struct ramfs_inode *is;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum];

	/* TODO Make sure exists beforehand */
	if (!is)
		return; //TODO Should notify of error somehow!

	is->type = ip->type;

	is->uid = ip->uid;
	is->gid = ip->gid;

	is->atime = ip->atime;
	is->ctime = ip->ctime;
	is->mtime = ip->mtime;

	is->size = ip->size;
}

/*
 * Read from an inode
 */
static int ramfs_read(struct inode *ip, void *data, off_t off, size_t n)
{
	struct ramfs_inode *is;

	if (n < 1)
		return 0;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum];

	if (!is || !is->data)
		return -1;

	memcpy(data, is->data + off, n);

	return 0;
}

/*
 * Write to an inode
 */
static int ramfs_write(struct inode *ip, const void *data, off_t off, size_t n)
{
	struct ramfs_inode *is;

	if (n < 1)
		return 0;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum];

	if (!is)
		return -1; //TODO

	if (!is->data)
		is->data = kmalloc(off + n);
	//else if (off + n > ip->size)
	//	is->data = krealloc(is->data, off + n);
	else if (off + n > ip->size) { /* XXX XXX XXX XXX XXX TEMP !!!! FIXME */
		void *p = kmalloc(off + n);
		if (!p)
			return -1;
			//TOERRNO return -ENOMEM;
		memcpy(p, is->data, is->size);
		kfree(is->data);
		is->data = p;
	}

	if (!is->data)
		return -1;
		//TOERRNO return -ENOMEM;

	memmove(is->data + off, data, n);

	if (off + n > ip->size) {
		ip->size = off + n;
		ramfs_write_inode(ip);
	}

	return 0;
}

/*
 * Create a file
 */
//TODO How do other Unix OSes do this?
static int ramfs_create(struct inode *dp, struct dirent *dep, mode_t mode)
{
	struct inode *ip;
	struct ramfs_dirent de;

	ip = dp->dev->op->alloc_inode(dp->dev);

	if (!ip)
		return -1;

	ip->mode = mode;

	dp->dev->op->write_inode(ip);

	return 0;
}

/*
 * Create a hard link
 */
static int ramfs_link(struct dirent *dep, struct inode *dp, const char *name)
{
	struct ramfs_dirent de;

	//if (odep->type == [directorytype] && odep->name != '.' or "..")
	//	return -1; // TODO
	//TODO Increase link count

	de.inum = dep->inum;
	de.type = dep->type;

	strncpy(de.name, name, RAMFS_NAME_MAX + 1);

	return ramfs_write(dp, &de, dp->size, sizeof(de));
	//TODO ERRNO
}

/*
 * Create a symbolic link
 */
/* TODO How does this crap work, how do you refer to other dirents? */
static int ramfs_symlink(struct inode *dp, struct dirent *dep, const char *name)
{
	struct ramfs_dirent de;

	de.inum = 0;
	de.type = IT_LINK;

	strncpy(de.name, name, RAMFS_NAME_MAX + 1);

	return ramfs_write(dp, &de, dp->size, sizeof(de));
}

/*static struct inode *ramfs_open(struct block_dev *dev, ino_t inum)
{
	struct inode *ip;

	ip = kmalloc(sizeof(struct inode));

	if (!ip)
		return NULL;

	ip->dev = dev;
	ip->inum = inum;

	ip->type = ((struct ramfs_inode *) dev->data[inum])->type;

	ip->refs = 1;
	ip->links = 0;

	ip->uid = ((struct ramfs_inode *) dev->data[inum])->uid;
	ip->gid = ((struct ramfs_inode *) dev->data[inum])->gid;

	ip->atime = ((struct ramfs_inode *) dev->data[inum])->atime;
	ip->ctime = ((struct ramfs_inode *) dev->data[inum])->ctime;
	ip->mtime = ((struct ramfs_inode *) dev->data[inum])->mtime;

	ip->size = ((struct ramfs_inode *) dev->data[inum])->size;

	return ip;
}*/

static struct dirent *ramfs_alloc_dir(struct ramfs_dirent *de)
{
	struct dirent *dep;

	dep = kmalloc(sizeof(struct dirent));

	if (!dep)
		return 0;

	memcpy(dep, de, sizeof(struct dirent));

	return dep;
}

/*
 * Read a directory entry by inode and offset
 */
//XXX TEMP without static
struct dirent *ramfs_read_dir(struct inode *dp, off_t off)
{
	struct ramfs_dirent de;
	int res;

	/* TODO Check in vfs.c */
	//if (dp->type != IT_DIR)
	//	return 0;
	
	if (off * sizeof(struct ramfs_dirent) > dp->size - 1)
		return NULL;

	res = ramfs_read(dp, &de, off * sizeof(struct ramfs_dirent),
			sizeof(struct ramfs_dirent));

	if (res < 0)
		return NULL;

	return ramfs_alloc_dir(&de);
}

///*
// * Create a new file
// */
//static int ramfs_touch(struct inode *pip, char *name, struct inode *ip)
//{
//	if (!name)
//		return -EINVAL;
//
//	//TODO Check if file already exists in directory
//
//	df
//}

/*
 * Create a new ramfs, size number of inodes
 * FIXME Number of inodes? That doesn't work like that I think...
 * TODO Dynamically change # of inodes based on free ram
 * TODO Temporarily returns ip
 */
int ramfs_get(size_t size, u32 *dev, struct inode **ipp)
{
	struct block_dev *dp;
	struct inode *ip;
	struct dirent *dep;
	int res;

	dp = kmalloc(sizeof(struct block_dev));

	if (!dp) {
		res = -ENOMEM;
		goto out;
	}

	dp->inum = 1; //FIXME Should be ip->inum, but ramfs_alloc_inode is broken
	dp->inodes = size;

	dp->op->alloc_inode = ramfs_alloc_inode;
	dp->op->write_inode = ramfs_write_inode;

	dp->op->create = ramfs_create;
	dp->op->link = ramfs_link;
	dp->op->symlink = ramfs_symlink;

	//dp->op->open = ramfs_open;
	//dp->op->read = ramfs_read;
	//dp->op->write = ramfs_write;

	ip = ramfs_alloc_inode(dp);
	dep = kmalloc(sizeof(struct dirent));

	if (!ip || !dep) {
		res = -ENOMEM;
		goto out;
	}

	ip->type = IT_DIR;
	ip->links = 1;

	dep->inum = ip->inum;
	dep->type = ip->type;

	res = ramfs_link(dep, ip, ".");

	if (res < 0)
		goto out;

	res = ramfs_link(dep, ip, "..");

	if (res < 0)
		goto out;

	//ramfs_update(ip);

	*dev = dp->dev;

	*ipp = ip;

	//struct dirent dep;
	//ramfs_read(ip, &dep, 0, 0);

	//return dep.name; /* FIXME Doesn't work, memcpy first */

	return 0;

out:
	kfree(dp);
	kfree(ip);
	kfree(dep);

	return res;
	//return NULL;
}
#endif

static const char devname[] = "ramfs";

static int ramfs_create(struct inode *dp, struct dirent *dep, mode_t mode)
{
	(void) dp, (void) dep, (void) mode;

	return 0;
}

static int ramfs_link(struct inode *dp, struct dirent *dep, struct dirent *name)
{
	(void) dp, (void) dep, (void) name;

	return 0;
}

static int ramfs_symlink(struct inode *dp, struct dirent *dep, const char *name)
{
	(void) dp, (void) dep, (void) name;

	return 0;
}

static int ramfs_rmlink(struct inode *dp, struct dirent *dep)
{
	(void) dp, (void) dep;

	return 0;
}

static int ramfs_mkdir(struct inode *dp, struct dirent *dep, mode_t mode)
{
	(void) dp, (void) dep, (void) mode;

	return 0;
}

static int ramfs_rmdir(struct inode *dp, struct dirent *dep)
{
	(void) dp, (void) dep;

	return 0;
}

static int ramfs_mknod(struct inode *dp, struct dirent *dep,
		mode_t mode, dev_t dev)
{
	(void) dp, (void) dep, (void) mode, (void) dev;

	return 0;
}

static int ramfs_move(struct inode *odp, struct dirent *odep,
		struct inode *dp, struct dirent *dep)
{
	(void) odp, (void) odep, (void) dp, (void) dep;

	return 0;
}

static struct sb_ops ramfs_sb_ops = {
	NULL
};

static struct inode_ops ramfs_inode_ops = {
	.create		= &ramfs_create,
	.link		= &ramfs_link,
	.symlink	= &ramfs_symlink,
	.rmlink		= &ramfs_rmlink,
	.mkdir		= &ramfs_mkdir,
	.rmdir		= &ramfs_rmdir,
	.mknod		= &ramfs_mknod,
	.move		= &ramfs_move
};

static struct file_ops ramfs_file_ops = {
	.read		= NULL,
	.write		= NULL
};

static struct file_ops ramfs_dir_ops = {
	.open		= NULL,
	.close		= NULL,
	.read		= NULL,
	.readdir	= NULL
};

static struct inode *ramfs_inode_alloc(struct superblock *sp,
		mode_t mode, dev_t dev)
{
	(void) dev;
	struct inode *ip;

	ip = inode_alloc(sp);

	if (!ip)
		return NULL;

	ip->mode = mode;

	switch (mode & IM_FTM) {
	case IM_DIR:
		ip->op = &ramfs_inode_ops;
		ip->fop = &ramfs_dir_ops;
		break;
	case IM_REG:
		ip->op = &ramfs_inode_ops;
		ip->fop = &ramfs_file_ops;
		break;
	default:
		/* TODO */
		break;
	}

	return ip;
}

/*static */struct superblock *ramfs_read_sb(struct superblock *sp)
{
	struct inode *ip;
	struct dirent *dep;

	sp->op = &ramfs_sb_ops;

	ip = ramfs_inode_alloc(sp, IM_DIR | 0755, (dev_t) { 0, 0 });

	if (!ip)
		return NULL;

	dep = dirent_alloc_root(ip);

	sp->root = dep;

	return sp;
}

static struct fs_driver ramfs_driver = {
	.name		= devname,

	.read_sb	= &ramfs_read_sb
};

void ramfs_init(void)
{
	fs_reg(&ramfs_driver);
}

void ramfs_exit(void)
{
	fs_unreg(&ramfs_driver);
	/* TODO */
}
