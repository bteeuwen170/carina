/*
 *
 * Elara
 * src/kernel/fs/dir.c
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
#include <limits.h>

#include <stdlib.h>
#include <string.h>

struct usr_dirent {
	ino_t	inum;
	char	name[NAME_MAX + 1];
};

struct dirent *dirent_alloc(struct dirent *dp, const char *name)
{
	struct dirent *dep;

	dep = kmalloc(sizeof(struct dirent));

	if (!dep)
		return NULL;

	if (strlen(name) > NAME_MAX) {
		kfree(dep);
		return NULL;
	}

	list_init(&dep->l);

	/* TODO Lock for this */
	if (dp)
		list_add(&dp->del, &dep->l);

	dep->ip = NULL;
	dep->dp = dp;
	list_init(&dep->del);

	dep->refs = 1;

	memcpy(dep->name, name, NAME_MAX);

	return dep;
}

struct dirent *dirent_alloc_root(struct inode *ip)
{
	struct dirent *dep;

	if (!ip)
		return NULL;

	dep = dirent_alloc(NULL, "/");

	if (!dep)
		return NULL;

	dep->ip = ip;
	dep->dp = dep;

	return dep;
}

void *dirent_get(struct file *fp)
{
	struct usr_dirent *udp;

	udp = kmalloc(sizeof(struct usr_dirent));

	udp->inum = fp->dp->ip->inum;
	memcpy(udp->name, fp->dp->name, sizeof(fp->dp->name));

	return udp;
}
