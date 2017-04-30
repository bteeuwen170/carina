/*
 *
 * Elarix
 * src/kernel/fs/dir.c
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
#include <list.h>
#include <proc.h>

#include <stdlib.h>
#include <string.h>

static const char devname[] = "fs";

static void _dir_put(struct dirent *dep)
{
	if (!dep)
		return;

	dep->refs--;

	if (dep->refs < 0)
		dprintf(KP_WARN "'%s', pointing to inode %d on %s "
				"has an invalid reference count: %d\n",
				dep->name, dep->inum, dep->sp->name, dep->refs);

	/* TODO Also do reverse sb_lookup to free */

	if (!(dep->sp->flags & M_KEEP) && !dep->refs)
		kfree(dep);
}

void dir_put(struct dirent *dep)
{
	struct dirent *cdep;

	/* FIXME Not thread safe */
	for (cdep = dep; cdep != fs_root; cdep = cdep->pdep)
		_dir_put(cdep);
}

int dir_get(const char *path, struct dirent **dep)
{
	struct superblock *sp;
	struct inode *dp = NULL;
	struct dirent *cdep = NULL, *pdep, *tdep = NULL;
	char fc, name[NAME_MAX + 1];
	int res, i;

	if ((fc = *path) == '/')
		cdep = fs_root;
	else
		cdep = cproc->cwd;

	while (*path) {
		while (*path == '/')
			path++;

		for (i = 0; *path && *path != '/'; i++)
			name[i] = *path++;
		name[i] = '\0';

		if (strcmp(name, "..") == 0) {
			if (cdep == cdep->pdep)
				continue;

			pdep = cdep;
			cdep = cdep->pdep;

			if (!*path) {
				pdep->refs++;
				_dir_put(pdep);
			}
		} else if (name[0] && strcmp(name, ".") != 0) {
			if ((res = inode_get(cdep->sp, cdep->inum, &dp)) < 0)
				return res;

			pdep = cdep;

			if ((res = dir_lookup(dp, name, &cdep)) < 0)
				goto err;

			res = sb_lookup(dp, name, &sp);
			if (res == 0) {
				/* TODO Check how safe this is */
				if (!(tdep = kmalloc(sizeof(struct dirent))))
					goto err;
				memcpy(tdep, cdep, sizeof(struct dirent));

				cdep = tdep;

				cdep->inum = sp->root->inum;
				cdep->sp = sp;
			} else if (res != -EINVAL) {
				goto err;
			}

			cdep->pdep = pdep;

			inode_put(dp);
		}

		dp = NULL;
		pdep = NULL;
	}

	for (tdep = cdep; tdep != fs_root; tdep = tdep->pdep)
		tdep->refs++;

	*dep = cdep;

	return 0;

err:
	if (pdep && pdep != ((fc == '/') ? fs_root : cproc->cwd)) {
		for (tdep = cdep; tdep != fs_root; tdep = tdep->pdep)
			tdep->refs++;

		dir_put(pdep);
	}
	if (dp)
		inode_put(dp);

	return res;
}

int dir_lookup(struct inode *dp, const char *name, struct dirent **dep)
{
	struct dirent *cdep;
	int res;

	list_for_each(cdep, &dp->del, l) {
		if (strcmp(cdep->name, name) == 0) {
			*dep = cdep;

			return 0;
		}
	}

	if ((res = dp->sp->fsdp->op->lookup(dp, name, &cdep)) < 0)
		return res;

	list_init(&cdep->l);
	list_add(&dp->del, &cdep->l);

	cdep->refs = 0;

	*dep = cdep;

	return 0;
}

int dir_basepath(char *path)
{
	char *sep;
	int i;

	if (!path)
		return -EINVAL;

	for (i = strlen(path) - 1; i && *(path + i) == '/'; i--);

	if ((sep = memrchr(path, '/', i)))
		*(sep + 1) = '\0';
	else
		*path = '\0';

	return 0;
}

int dir_basename(char *path)
{
	char *sep;
	int i;

	if (!path)
		return -EINVAL;

	for (i = strlen(path) - 1; i && *(path + i) == '/'; i--);
	*(path + i + 1) = '\0';

	if ((sep = memrchr(path, '/', i)))
		memmove(path, sep + 1, i + 1);

	return 0;
}
