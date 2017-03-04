/*
 *
 * Elarix
 * src/kernel/fs/dev.c
 *
 * Copyright (C) 2017 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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
#include <limits.h>

#include <stdlib.h>
#include <string.h>

static struct device devices[DEV_MAX];

int dev_reg(u8 major, const char *name, struct file_ops *op)
{
	if (strlen(name) > NAME_MAX)
		return -EINVAL;

	if (devices[major].op)
		return -EEXIST;

	devices[major].name = name;
	devices[major].op = op;

	return 0;
}

void dev_unreg(u8 major)
{
	devices[major].name = NULL;
	devices[major].op = NULL;
}

int dev_init(dev_t dev)
{
	struct dirent *dp, *dep;
	char name[NAME_MAX + 1];
	int res;

	sprintf(name, "%s%d", devices[dev.major].name, dev.minor);

	if (!(dp = dirent_get("/sys/dev"))) {
		if ((res = sv_mkdir("/sys/dev", 0)) != 0)
			if ((res = sv_mkdir("/sys", 0)) == 0)
				if ((res = sv_mkdir("/sys/dev", 0)) != 0)
					return res;

		if (!(dp = dirent_get("/sys/dev")))
			return -1; /* TODO */
	}

	if (!(dep = dirent_alloc(dp, name)))
		return -ENOMEM;
	dep->ip = inode_alloc(root_sb);
	dep->ip->fop = devices[dev.major].op;

	return 0;
}
