/*
 *
 * Elarix
 * src/kernel/fs/inode.c
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

#include <fs.h>
#include <proc.h>

#include <stdlib.h>

static const char devname[] = "fs";

ino_t inodes = 0;

/* static int write_inode(struct inode *ip)
{
	[>TODO<]
} */

static void inode_delete(struct inode *ip)
{
	if (ip->sp->op->inode_delete) {
		ip->sp->op->inode_delete(ip);
	} else {
		/* TODO */
	}
}

struct inode *inode_alloc(struct superblock *sp)
{
	struct inode *ip;

	if (sp->op->inode_alloc)
		ip = sp->op->inode_alloc(sp);
	else
		ip = kcalloc(1, sizeof(struct inode));
	if (!ip)
		return NULL;

	list_init(&ip->l);

	if (!ip->inum)
		ip->inum = ++inodes;
	/* ip->flags = 0; */

	ip->refs = 1;

	ip->dev = sp->dev;
	/* TODO Set to current user */
	ip->uid = cproc->uid;
	ip->gid = cproc->gid;

	/* TODO Get current time */
	if (!ip->atime)
		ip->atime = 60;
	if (!ip->ctime)
		ip->ctime = 60;
	if (!ip->mtime)
		ip->mtime = 60;

	ip->sp = sp;
	list_init(&ip->del);

	return ip;
}

static void inode_dealloc(struct inode *ip)
{
	/* struct dirent *dep; */

	if (!ip->links)
		inode_delete(ip);

	if (ip->sp->flags & SF_KEEP)
		return;

	/* list_for_each(dep, &ip->del, l) {
		dirent_put
	} */

	if (ip->sp->op->inode_dealloc)
		ip->sp->op->inode_dealloc(ip);
	else
		kfree(ip);
}

struct inode *inode_get(struct superblock *sp, ino_t inum)
{
	struct inode *ip;

	list_for_each(ip, &sp->il, l)
		if (ip->inum == inum)
			return ip;

	return inode_alloc(sp);
}

void inode_put(struct inode *ip)
{
	if (!ip)
		return;

	ip->refs--;

	if (!ip->refs)
		inode_dealloc(ip);
}
