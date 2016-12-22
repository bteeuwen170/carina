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

#include <stdlib.h>

ino_t inodes = 0;

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

	ip->dev = sp->dev;
	if (!ip->inum)
		ip->inum = ++inodes;
	ip->mode = 0;

	ip->refs = 1;
	ip->links = 1;

	/* TODO Set to current user */
	ip->uid = 0;
	ip->gid = 0;

	/* TODO Get current time */
	ip->atime = 0;
	ip->ctime = 0;
	ip->mtime = 0;

	ip->size = 0;

	ip->sp = sp;

	return ip;
}

void inode_dealloc(struct inode *ip)
{
	if (ip->sp->op->dealloc_inode)
		ip->sp->op->dealloc_inode(ip);
	else
		kfree(ip);
}
