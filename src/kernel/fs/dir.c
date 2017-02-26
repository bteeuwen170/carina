/*
 *
 * Elarix
 * src/kernel/fs/dir.c
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
#include <limits.h>
#include <proc.h>

#include <stdlib.h>
#include <string.h>

struct dirent *dirent_alloc(struct dirent *dp, const char *name)
{
	struct dirent *dep;

	if (dp)
		list_for_each(dep, &dp->ip->del, l)
			if (strcmp(dep->name, name) == 0)
				return NULL;

	if (!(dep = kmalloc(sizeof(struct dirent))))
		return NULL;

	if (strlen(name) > NAME_MAX) {
		kfree(dep);
		return NULL;
	}

	list_init(&dep->l);
	if (dp)
		list_add(&dp->ip->del, &dep->l);

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

	if (!(dep = dirent_alloc(NULL, "/")))
		return NULL;

	dep->ip = ip;
	dep->dp = dep;

	return dep;
}

static void dirent_dealloc(struct dirent *dep)
{
	/* if (!dep->ip->links)
		inode_delete(ip);

	if (ip->sp->op->dealloc_inode)
		ip->sp->op->dealloc_inode(ip);
	else
		kfree(ip); */
}

struct dirent *dirent_get(const char *path)
{
	struct dirent *dep, *dec;
	char name_buf[NAME_MAX + 1];
	int i = 0;

	/* Incorrect */
	/*dep = kmalloc(sizeof(struct dirent));
	if (!dep)
		return NULL;*/

	if (*path == '/') {
		dep = root_sb->root;
		path++;
	} else {
		dep = cproc->cwd;
	}

	/* inode_put parent */

	while (*path) {
		memset(name_buf, 0, NAME_MAX + 1);

		while (*path && *path != '/')
			name_buf[i++] = *(path++);

		if (strcmp(name_buf, "..") == 0) {
			dep = dep->dp;
		} else if (strcmp(name_buf, ".") != 0) {
			list_for_each(dec, &dep->ip->del, l) {
				if (strcmp(dec->name, name_buf) == 0) {
					dep = dec;
					goto ret;
				}
			}

			return NULL;
		}

		if (*path == '/')
			path++;
		i = 0;
	}

ret:
	dep->refs++;

	return dep;
}

struct usr_dirent *usr_dirent_get(struct file *fp)
{
	struct usr_dirent *udp;
	struct dirent *dep;
	int i = 0;

	if (!(udp = kmalloc(sizeof(struct usr_dirent))))
		return NULL;

	list_for_each(dep, &fp->dep->ip->del, l) {
		if (i++ == fp->off) {
			fp->off++;

			udp->inum = dep->ip->inum;
			strncpy(udp->name, dep->name, NAME_MAX);

			return udp;
		}
	}

	return NULL;
}

void dirent_put(struct dirent *dep)
{
	dep->refs--;

	if (!dep->refs)
		dirent_dealloc(dep);
}
