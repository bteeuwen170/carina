/*
 *
 * Elarix
 * src/kernel/dev/block/buf.c
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

#include <block.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#ifdef CONFIG_BLOCK_VERBOSE
#include <kernel.h>
#endif
#include <list.h>
#include <mm.h>

static LIST_HEAD(blocks);

int block_get(dev_t dev, off_t block, struct block **bp)
{
	struct block *cbp;
	struct device *devp;
	struct inode ip;
	struct file fp;
	int res;

	if (!(devp = device_get(dev)))
		return -ENODEV;

	if (!(cbp = kmalloc(sizeof(struct block), 0)))
		return -ENOMEM;

	list_init(&cbp->l);
	list_add(&blocks, &cbp->l);
	cbp->refs = 1;

	cbp->dev = dev;
	cbp->block = block;
	cbp->flags = 0;

	fp.ip = &ip;
	fp.ip->rdev = dev;

	if ((res = devp->op->read(&fp, cbp->buffer, cbp->block, 1)) < 0)
		goto err;

#ifdef CONFIG_BLOCK_VERBOSE
	kprintf("Block buffer allocated @ %#x\n", cbp);
#endif

	*bp = cbp;

	return res;

err:
	kfree(cbp);

	return res;
}

void block_put(struct block *bp)
{
	if (!bp)
		return;

#ifdef CONFIG_BLOCK_VERBOSE
	kprintf("Block buffer deallocated @ %#x\n", bp);
#endif

	list_rm(&bp->l);
	kfree(bp);
}
