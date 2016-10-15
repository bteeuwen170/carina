/*
 *
 * Carina
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

#include <stdlib.h>
#include <string.h>
#include <kernel/types.h> /* XXX TEMP for syntax highlighting */
#include <kernel/print.h>
#include <kernel/limits.h>
#include <sys/err.h>

#include <sys/fs.h>

/* Block size */
//#define RAMFS_BS	0

/* Inodes per block */
//#define RAMFS_IPB	(RAMFS_BS / sizeof(struct ramfs_inode))

/* Inode */
//#define INODE(inum, sb)	((inum) / RAMFS_IPB + sb.fb)

struct ramfs_dirent {
	u64	inum;
	char	name[NAME_MAX];
};

struct ramfs_inode {
	u8	type;
//	u16	perms;

	u32	links;

	uid_t	uid;
	gid_t	gid;

	time_t	atime;
	time_t	ctime;
	time_t	mtime;

	u64	size;

	struct ramfs_dirent	*dep;
	void			*data;
};

static struct inode *ramfs_get(struct block_dev *dev, u64 inum)
{
	struct inode *ip;

	ip = kmalloc(sizeof(struct inode));

	if (!ip)
		return 0;

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
}

/*
 * Create an inode
 */
static struct inode *ramfs_alloc(struct block_dev *dev)
{
	u64 i;

	/* TODO Don't hardcode first block */
	for (i = 1; i < dev->inodes; i++) {
		if (dev->data[i] != NULL)
			continue;

		dev->data[i] = kmalloc(sizeof(struct ramfs_inode));

		return ramfs_get(dev, i);
	}

	/* FIXME Doesn't work */
	return 0;

	//return -ENOSPC;
}

/*
 * Read from an inode
 */
static int ramfs_read(struct inode *ip, void *data, u64 off, u64 n)
{
	struct ramfs_inode *is;

	if (n < 1)
		return 0;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum];

	if (is->data != NULL)
		memcpy(data, is->data + off, n);

	return 0;
}

/*
 * Update an inode
 */
static int ramfs_update(struct inode *ip)
{
	struct ramfs_inode *is;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum];

	if (is == NULL)
		return -1; //TODO Return proper value

	is->type = ip->type;

	is->uid = ip->uid;
	is->gid = ip->gid;

	is->atime = ip->atime;
	is->ctime = ip->ctime;
	is->mtime = ip->mtime;

	is->size = ip->size;

	return 0;
}

/*
 * Write to an inode
 */
static int ramfs_write(struct inode *ip, void *data, u64 off, u64 n)
{
	struct ramfs_inode *is;

	if (n < 1)
		return 0;

	is = (struct ramfs_inode *) ip->dev->data[ip->inum + 1];

	//dp = kmalloc(sizeof(struct block_dev));

	if (!is)
		return -1; //TODO Create new inode

	/* FIXME */
	//is->data = data;
	memcpy(is->data + off, data, n);

	if (off > ip->size) {
		ip->size = off;
		ramfs_update(ip);
	}

	return 0;
}

/*
 * Link an inode to a directory by inode number
 */
static int ramfs_dir_write(struct inode *dp, u64 inum, char *name)
{
	struct ramfs_dirent dep;
	int res;
	u64 i = 0;

	do {
		struct ramfs_inode *is = (struct ramfs_inode *) dp->dev->data[inum];

		if (!is)
			return -1;

		if (!is->dep)
			return -12;

		if (strcmp(dep.name, name) == 0)
			return -EEXIST;
	} while (dep.inum != 0);

	dep.inum = inum;
	strncpy(dep.name, name, NAME_MAX);

	return 0;
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
 */
int ramfs_create(u64 size, u32 *dev)
{
	struct block_dev *dp;
	struct inode *ip;
	int res;

	dp = kmalloc(sizeof(struct block_dev));

	if (!dp) {
		res = -ENOMEM;
		goto out;
	}

	dp->inum = 1;
	dp->inodes = size;

	dp->op->alloc = ramfs_alloc;
	dp->op->read = ramfs_read;
	dp->op->write = ramfs_write;

	dev_reg(dp);

	ip = ramfs_alloc(dp);

	if (!ip) {
		res = -ENOMEM;
		goto out;
	}

	ip->dev = dp;
	ip->inum = dp->inum;

	ip->type = IT_DIR;

	ip->refs = 0;
	ip->links = 1;

	res = ramfs_dir_write(ip, ip->inum, ".");

	if (res < 0)
		goto out;

	res = ramfs_dir_write(ip, ip->inum, "..");

	if (res < 0)
		goto out;

	ramfs_update(ip);

	*dev = dp->dev;

	//struct dirent dep;
	//ramfs_read(ip, &dep, 0, 0);

	//return dep.name; /* FIXME Doesn't work, memcpy first */

	return 0;

out:
	kfree(dp);
	kfree(ip);

	return res;
	//return NULL;
}
