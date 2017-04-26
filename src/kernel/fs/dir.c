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
#include <list.h>
#include <proc.h>

#include <stdlib.h>
#include <string.h>

static int dir_lookup(struct inode *dp, const char *name, struct dirent **dep)
{
	struct dirent *cdep;
	int res;

	list_for_each(cdep, &dp->del, l) {
		if (cdep->inum == dp->inum && strcmp(cdep->name, name) == 0) {
			cdep->refs++;

			*dep = cdep;

			return 0;
		}
	}

	if ((res = dp->sp->fsdp->op->lookup(dp, name, &cdep)) < 0)
		return res;

	*dep = cdep;

	return 0;
}

int dir_get(const char *path, struct dirent **dep)
{
	struct superblock *sp;
	struct inode *dp = NULL;
	struct dirent *pdep, *cdep = NULL;
	char name[NAME_MAX + 1];
	int res, i;

	if (*path == '/')
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
			pdep = cdep;
			cdep = cdep->pdep;

			dir_put(pdep);
		} else if (name[0] && strcmp(name, ".") != 0) {
			if ((res = inode_get(cdep->sp, cdep->inum, &dp)) < 0)
				return res;

			pdep = cdep;

			if ((res = dir_lookup(dp, name, &cdep)) < 0)
				return res;

			if ((sp = sb_lookup(cdep))) {
				cdep->inum = sp->root->inum;
				cdep->sp = sp;
			}

			cdep->pdep = pdep;

			inode_put(dp);
		}
	}

	*dep = cdep;

	return 0;
}

void dir_put(struct dirent *dep)
{
	if (!dep)
		return;

	if (dep->pdep != fs_root)
		dir_put(dep->pdep);

	/* if (!dep->refs < 0)
		panic("dirent %d (%s) has a invalid reference count",
				dep->inum, dep->name); */

	if (!dep->refs)
		kfree(dep);

	kfree(dep);
}
