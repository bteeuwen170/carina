/*
 *
 * Elarix
 * src/kernel/fs/super.c
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
#include <string.h>

static LIST_HEAD(superblocks);

int sb_get(struct fs_driver *fsdp, dev_t dev, u8 flags, struct superblock **sp)
{
	struct superblock *csp;
	int res;

	list_for_each(csp, &superblocks, l) {
		if (csp->dev == dev) {
			*sp = csp;

			return 0;
		}
	}

	if (!(csp = kmalloc(sizeof(struct superblock))))
		return -ENOMEM;

	list_init(&csp->l);

	csp->dev = dev;
	csp->name[0] = '\0';
	csp->flags = flags;

	csp->blocks = 0;
	csp->block_size = 0;

	list_init(&csp->bl);
	list_init(&csp->il);

	csp->root = NULL;
	csp->pdep = NULL;

	csp->fsdp = fsdp;

	if ((res = fsdp->op->sb_get(csp)) < 0)
		goto err;

	list_add(&superblocks, &csp->l);

	*sp = csp;

	return 0;

err:
	if (csp->root)
		inode_put(csp->root);
	kfree(csp);

	return res;
}

int sb_put(struct superblock *sp)
{
	struct superblock *csp;
	int res;

	/* TODO Clear block and inode cache */

	list_for_each(csp, &superblocks, l) {
		if (csp->dev != sp->dev)
			continue;

		if ((res = csp->fsdp->op->sb_put(csp)) < 0)
			return res;

		inode_put(csp->root);
		dir_put(csp->pdep);

		list_rm(&csp->l);

		kfree(csp);

		return res;
	}

	return -EINVAL;
}

struct superblock *sb_lookup(struct dirent *dep)
{
	struct superblock *csp;

	/* XXX Is this completely safe? */
	list_for_each(csp, &superblocks, l)
		if (!csp->pdep)
			continue;
		else if ((csp->pdep->sp == dep->sp &&
				csp->pdep->inum == dep->inum) ||
				(csp == dep->sp &&
				csp->root->inum == dep->inum))
			return csp;

	return NULL;
}
