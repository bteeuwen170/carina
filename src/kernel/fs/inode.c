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
#include <kernel.h>
#include <mm.h>

#include <string.h>

static const char devname[] = "fs";

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
	if (!ip)
		return;

	ip->refs--;

	if (ip->refs < 0)
		dprintf(KP_WARN "inode %lu on %s "
				"has an invalid reference count: %d\n",
				ip->inum, ip->sp->name, ip->refs);

	if (!(ip->sp->flags & M_KEEP) && !ip->refs) {
		list_rm(&ip->l);
		kfree(ip);
	}
}

int inode_dirisempty(struct inode *dp)
{
	struct file fp;
	char buf[NAME_MAX + 1];
	size_t cnt = 0;

	if (!(dp->mode & I_DIR))
		return -ENOTDIR;

	if (!dp->op->readdir)
		return -EPERM;

	fp.mode = F_RO | F_DIR;
	fp.ip = dp;

	while (dp->op->readdir(&fp, buf) == 0)
		if (strcmp(buf, ".") == 0 || strcmp(buf, "..") == 0)
			cnt++;

	return (!cnt || cnt == 2) ? 1 : 0;
}
