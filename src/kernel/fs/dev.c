/*
 *
 * Elara
 * src/kernel/fs/dev.c
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

#include <errno.h>
#include <fs.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>

struct list_head devices[DEV_MAX];

int dev_block_reg(u8 major, struct fs_dev *device)
{
	return -EINVAL;
}

int dev_char_reg(u8 major, struct fs_dev *device)
{
	/* TODO Check if not already present */

	list_init(&device->l);
	list_add(&devices[major], &device->l);

	return 0;
}

int dev_char_unreg(u8 major, const char *name)
{
	struct fs_dev *device;

	/* TODO Check for presence */

	list_for_each(device, &devices[major], l)
		if (strcmp(device->name, name) == 0) {
			list_rm(&device[major].l);
			kfree(device);
		}

	return 0;
}

void dev_init(void)
{
	int i;

	for (i = 0; i < DEV_MAX; i++)
		list_init(&devices[i]);
}
