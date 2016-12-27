/*
 *
 * Elarix
 * src/kernel/fs/inode.c
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

#include <fs.h>
#include <proc.h>

#include <stdlib.h>

static const char devname[] = "fs";

ino_t inodes = 0;

static void inode_delete(struct inode *ip)
{
	if (ip->sp->op->delete_inode) {
		ip->sp->op->delete_inode(ip);
	} else {
		/* TODO */
	}
}

struct inode *inode_alloc(struct superblock *sp)
{
	struct inode *ip;

	if (sp->op->alloc_inode)
		ip = sp->op->alloc_inode(sp);
	else
		ip = kmalloc(sizeof(struct inode));

	if (!ip)
		return NULL;

	list_init(&ip->l);

	if (!ip->inum)
		ip->inum = ++inodes;
	/* ip->flags = 0; */
	ip->mode = 0;

	ip->refs = 1;
	ip->links = 0;

	/* TODO Set to current user */
	ip->uid = cproc->uid;
	ip->gid = cproc->gid;

	/* TODO Get current time */
	ip->atime = 0;
	ip->ctime = 0;
	ip->mtime = 0;

	ip->size = 0;

	ip->sp = sp;
	list_init(&ip->del);

	ip->op = NULL;
	ip->fop = NULL;

	return ip;
}

static void inode_dealloc(struct inode *ip)
{
	if (!ip->links)
		inode_delete(ip);

	if (ip->sp->flags & SF_KEEP)
		return;

	if (ip->sp->op->dealloc_inode)
		ip->sp->op->dealloc_inode(ip);
	else
		kfree(ip);
}

struct inode *inode_get(struct superblock *sp, ino_t inum)
{
	struct inode *ip;

	list_for_each(ip, &sp->il, l) {
		if (ip->inum == inum)
			return ip;
	}

	return inode_alloc(sp);
}

void inode_put(struct inode *ip)
{
	ip->refs--;

	if (!ip->refs)
		inode_dealloc(ip);
}
