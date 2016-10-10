/*
 *
 * Carina
 * src/kernel/fs/fs.c
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

struct block_dev *block_devices;
u32 devcount = 0;

//struct mountp *mount_points;

void dev_reg(struct block_dev *dp)
{
	dp->dev = ++devcount;
	dp->next = block_devices;

	block_devices = dp;
}

//struct dev_block *block_devices[1]; /* XXX TEMP */

/*
 * Directories
 * TODO Seperate file
 */

/*
 * Look for a directory entry by name
 */
//int dir_search(struct inode *ip, char *name, u64 *off)
//{
//	if (ip->type != FT_DIR)
//		return -ENOTDIR;
//
//	for (i = 0; i < ip->size; i += sizeof(struct dir)) {
//		
//	}
//
//	return -ENOENT;
//}

/*
 * Seek directory entry by name
 */
//struct dp *dir_read(struct inode *dp, char *name) {
//
//}

/*
 * Link an inode to a directory by inode number
 * FIXME In file system handler
 */
int dir_write(struct inode *dp, u64 inum, char *name)
{
	struct dirent dep;
	int res;
	u64 i;

	dep.inum = 0;

	for (i = 0; dep.inum != 0; i += sizeof(dep)) {
		//if (i >= dp->size)
		//	return -ENOSPC;

		res = dp->dev->op->read(dp, &dep, i, sizeof(dep));

		if (res < 0)
			return res;

		/* TODO Use strncmp ? */
		if (strcmp(dep.name, name) == 0)
			return -EEXIST;
	}

	dep.inum = inum;
	strncpy(dep.name, name, NAME_MAX);

	res = dp->dev->op->write(dp, &dep, i, sizeof(dep));

	if (res < 0)
		return res;

	return 0;
}

/*
 * Inodes
 * TODO Seperate fie
 */

struct inode inode_cache[INODES_MAX];

/* Where does this go? */
/*static struct inode *inode_create(char *path)
{
	struct inode *ip;

	
}*/

//static struct inode *inode_get(u32 dev, u64 inum)
//{
//	struct inode *ip;
//
//
//}

//static struct inode *inode_getn(char *path)
//{
//	struct inode *ip;
//
//	if (*path == '/')
//		ip = dev->; /* TODO Not always 1 for device */
//}

//void inode_drop(struct inode *ip)
//{
//	if (ip->refs == 1 && ip->links < 1) {
//		/* TODO Free memory inode */
//	}
//
//	ip->refs--;
//}

/*
 * Files
 * TODO Seperate fie
 */

//void file_read(struct file *f, size_t size)
//{
//	/* TODO Check if not read-only */
//
//	//inode->atime = 0; /* TODO Only if noatime is not set */
//
//	//return f->
//}
//
//void file_readdir(struct file *f, size_t size)
//{
//	if (!(f->type & FT_DIR))
//		return -EINVAL;
//}
//
//void file_open(struct file *f, int flags)
//{
//	//if (f
//}
//
//int fs_init(void)
//{
//	/* Temporary for vfs */
//	inode_alloc(0, 1);
//}

/*
 * System calls
 * TODO Seperate file
 */

/* FIXME HACKS FIXME */
//int sys_touch(char *path, u8 type, struct inode *temp) /* XXX Remove last arg */
//{
//	struct inode *ip;
//	int res;
//
//	/* TODO Get parent inode using inode_getn */
//
//	struct super *temp2 = malloc(sizeof(struct super));
//	temp2->inodes = 4096;
//
//	res = temp->op->alloc(temp2, &ip)
//
//	if (res < 0)
//		return res;
//
//	if (type == IT_DIR) {
//		//TODO Create . and ..
//	}
//
//	//TODO Link to parent
//}
//
//int sys_mkdir(char *path)
//{
//	//TODO ip->links++;
//
//	return sys_touch(path, FT_DIR);
//}
