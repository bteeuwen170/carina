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

#include <errno.h>
#include <fs.h>

#include <stdlib.h>

int inode_get(struct superblock *sp, ino_t inum, struct inode **ip)
{
	struct inode *cip;
	int res;

	list_for_each(cip, &sp->il, l) {
		if (cip->inum == inum) {
			cip->refs++;

			*ip = cip;

			return 0;
		}
	}


	if (!(cip = kcalloc(1, sizeof(struct inode))))
		return -ENOMEM;

	list_init(&cip->l);

	cip->inum = inum;
	cip->refs = 1;
	cip->links = 1;
	cip->sp = sp;

	list_init(&cip->del);

	if ((res = sp->fsdp->op->alloc(cip)) < 0)
		goto err;

	list_add(&sp->il, &cip->l);

	*ip = cip;

	return 0;

err:
	kfree(cip);

	return res;
}

void inode_put(struct inode *ip)
{
	/* TODO */
}

int inode_dirisempty(struct inode *dp)
{
	/* TODO */
	return 1;
}
