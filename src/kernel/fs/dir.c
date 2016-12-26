/*
 *
 * Elarix
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

#include <errno.h>
#include <fs.h>
#include <limits.h>
#include <proc.h>

#include <stdlib.h>
#include <string.h>

static struct dirent *dirent_alloc(struct dirent *dp, const char *name)
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

	strncpy(dep->name, name, NAME_MAX);

	dep->refs = 1;

	dep->ip = NULL;
	dep->dp = dp;

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

struct dirent *dirent_get(const char *path)
{
	struct dirent *dep;

	/* Incorrect */
	/*dep = kmalloc(sizeof(struct dirent));
	if (!dep)
		return NULL;*/

	if (*path == '/')
		dep = root_sb->root;
	else
		dep = cproc->cwd;

	/* while (*path) {

	} */

	return dep;
}

struct usr_dirent *usr_dirent_get(struct file *fp)
{
	struct usr_dirent *udp;

	udp = kmalloc(sizeof(struct usr_dirent));

	udp->inum = fp->dep->ip->inum;
	strncpy(udp->name, fp->dep->name, NAME_MAX);

	return udp;
}
