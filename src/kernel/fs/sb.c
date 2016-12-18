/*
 *
 * Elara
 * src/kernel/fs/sb.c
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
#include <list.h>

#include <stdlib.h>

static LIST_HEAD(superblock);

struct superblock *sb_alloc(struct fs_driver *driver)
{
	struct superblock *sp;

	/* TODO Check if doesn't exist already */

	sp = kmalloc(sizeof(struct superblock));

	if (!sp)
		return NULL;

	list_init(&sp->l);

	sp->dev = (dev_t) { 0, 0 };

	list_init(&sp->il);
	sp->inodes = 0;

	return sp;
}
