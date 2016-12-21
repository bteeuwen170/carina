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

static struct fs_dev devices[DEV_MAX];

int dev_block_reg(u32 major, struct fs_dev *device)
{
	return -EINVAL;
}

int dev_char_reg(u32 major, const char *name, struct file_ops *op)
{
	/* TODO Lock */

	if (major > DEV_MAX)
		return -EINVAL;

	if (devices[major].op != op)
		return -EEXIST;

	memcpy(devices[major].name, name, NAME_MAX);
	devices[major].op = op;

	return 0;
}

void dev_char_unreg(u32 major)
{
	memset(devices[major].name, 0, NAME_MAX + 1);
	devices[major].op = NULL;
}
