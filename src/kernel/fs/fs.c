/*
 *
 * Elarix
 * src/kernel/fs/fs.c
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

#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <kernel.h>
#include <proc.h>

#include <string.h>

static const char devname[] = "fs";

static LIST_HEAD(fs_drivers);

static struct dirent _fs_root = {
	.refs	= 0,

	.inum	= 0,
	.name	= "/",

	.sp	= NULL,
	.pdep	= &_fs_root
};

struct dirent *fs_root = &_fs_root;

int fs_chdir(const char *path)
{
	struct inode *dp = NULL;
	struct dirent *dep = NULL;
	int res;

	if ((res = dir_get(path, &dep)) < 0)
		goto err;

	if ((res = inode_get(dep->sp, dep->inum, &dp)) < 0)
		goto err;

	if (!(dp->mode & I_DIR)) {
		res = -ENOTDIR;
		goto err;
	}

	/* FIXME More check 'n stuff */

	cproc->cwd = dep;

	return 0;

err:
	if (dp)
		inode_put(dp);
	if (dep)
		dir_put(dep);

	return res;
}

int fs_cwdir(char *_path)
{
	struct dirent *dep;
	char path[PATH_MAX], *pb, *nb;

	dep = cproc->cwd;
	path[0] = '\0';

	if (!_path)
		return -EINVAL;

	do {
		strcat(path, dep->name);
		if (*path != '/')
			strcat(path, "/");

		dep = dep->pdep;
	} while (dep != fs_root);

	pb = nb = path;

	while (*nb++) {
		if (*nb == '\0') {
			strnrev(pb, nb - pb);
		} else if (*nb == '/') {
			strnrev(pb, nb - pb);
			pb = nb + 1;
		}
	}

	strrev(path);
	strcpy(_path, path);

	return 0;
}

int fs_mount(dev_t dev, const char *path, const char *fs, u8 flags)
{
	struct fs_driver *fsdp;
	struct superblock *sp = NULL;
	struct inode *dp = NULL;
	struct dirent *dep = NULL;
	int res;

	if (dev && (MINOR(dev) != MAJOR_DSK && MAJOR(dev) != MAJOR_OPT))
		return -ENOTBLK;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	list_for_each(fsdp, &fs_drivers, l)
		if (strcmp(fsdp->name, fs) == 0)
			goto foundfs;

	return -EINVAL;

foundfs:
	if (fs_root->refs || strcmp(path, "/") != 0) {
		if ((res = dir_get(path, &dep)) < 0)
			goto err;

		if ((res = inode_get(dep->sp, dp->inum, &dp)) < 0)
			goto err;

		if (!(dp->mode & I_DIR)) {
			res = -ENOTDIR;
			goto err;
		}

		if (!inode_dirisempty(dp)) {
			res = -EBUSY;
			goto err;
		}

		dp->links--;

		inode_put(dp);
	}

	if ((res = sb_get(fsdp, dev, flags, &sp)) < 0)
		goto err;

	if (strcmp(path, "/") == 0) {
		list_init(&fs_root->l);
		fs_root->refs++;
		fs_root->inum = sp->root->inum;
		fs_root->sp = sp;
		list_add(&sp->root->del, &fs_root->l);

		cproc->cwd = fs_root;
	} else {
		sp->pdep = dep;
		/* list_add(&sp->root->del, &dep->l); */
	}

	dprintf(devname, "%s (%s) has been successfully mounted on %s\n",
			sp->name, fs, path);
	if (!dev)
		return 0;

	dprintf(devname, "%d blocks, %d MB (%d bytes per block)\n", sp->blocks,
			(sp->blocks * sp->block_size) / 1024, sp->block_size);

	return 0;

err:
	if (sp)
		sb_put(sp);
	if (dp)
		inode_put(dp);
	if (dep)
		dir_put(dep);

	return res;
}

int fs_mkreg(const char *path, mode_t mode)
{
	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (mode >= I_LINK)
		return -EINVAL;

	/* TODO */

	return -1;
}

int fs_mkdir(const char *path, mode_t mode)
{
	struct dirent *dep = NULL;
	char pbuf[PATH_MAX], *sep;
	int res, i;

	pbuf[0] = '\0';

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (mode >= I_LINK)
		return -EINVAL;

	for (i = strlen(path) - 1; i && *(path + i) == '/'; i--);

	if ((sep = memrchr(path - 1, '/', i)))
		strncat(pbuf, sep + 2, i - (sep - path));
	else
		strncat(pbuf, path, i + 1);

	/* TODO */

	if ((res = dir_get(path, &dep)) < 0)
		goto err;

	if (dep->sp->flags & M_RO) {
		res = -EROFS;
		goto err;
	}

	return 0;

err:
	if (dep)
		dir_put(dep);

	return res;
}

/* int fs_mkdev(const char *path, mode_t mode, dev_t dev)
{
	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (mode >= I_LINK)
		return -EINVAL;
} */

/* int fs_mklnk(const char *src_path, const char *path)
{
	if (strlen(src_path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;
} */

/* int fs_rmlnk(const char *path)
{
	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

} */

/* int fs_link(const char *src_path, const char *path)
{
	if (strlen(src_path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;
} */

/* int fs_move(const char *src_path, const char *path)
{
	if (strlen(src_path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;
} */

int fs_reg(struct fs_driver *fsdp)
{
	struct fs_driver *cfsdp;

	list_for_each(cfsdp, &fs_drivers, l)
		if (strcmp(cfsdp->name, fsdp->name) == 0)
			return -EEXIST;

	list_init(&fsdp->l);
	list_add(&fs_drivers, &fsdp->l);

	return 0;
}

void fs_unreg(struct fs_driver *dfsdp)
{
	list_rm(&dfsdp->l);
}
