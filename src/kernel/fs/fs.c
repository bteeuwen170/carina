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

#include <cmdline.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <kernel.h>
#include <proc.h>

#include <stdlib.h>
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
	struct superblock *sp;
	struct inode *dp = NULL;
	struct dirent *dep = NULL;
	int res;

	/* TODO Check if already mounted elsewhere */

	if ((MAJOR(dev) != MAJOR_MEM &&
			MAJOR(dev) != MAJOR_HDD && MAJOR(dev) != MAJOR_ODD))
		return -EINVAL;

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

		if ((res = inode_get(dep->sp, dep->inum, &dp)) < 0)
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

		cproc->cwd = fs_root;
	} else {
		sp->pdep = dep;
	}

	dprintf(devname, "%s (%s) has been successfully mounted on %s\n",
			sp->name, fs, path);
	if (MAJOR(dev) != MAJOR_HDD && MAJOR(dev) != MAJOR_ODD)
		return 0;

	dprintf(devname, KP_CON "%d blocks, %d MB (%d bytes per block)\n",
			sp->blocks,
			(sp->blocks * sp->block_size) / 1024, sp->block_size);

	return 0;

err:
	if (dp)
		inode_put(dp);
	if (dep)
		dir_put(dep);

	return res;
}

int fs_unmount(const char *path)
{
	struct dirent *dep;
	int res;

	/* TODO Run some more checks first */

	if (strcmp(path, "/") == 0)
		panic("attempted to unmount root", 0, 0);

	if ((res = dir_get(path, &dep)))
		return res;

	if ((res = sb_put(dep)) < 0)
		goto err;

	dir_put(dep);

	return 0;

err:
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
	struct inode *dp = NULL;
	struct dirent *ddep = NULL, *dep = NULL;
	char buf[PATH_MAX + 1];
	int res;

	if (!path)
		return -EINVAL;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (mode >= I_LINK)
		return -EINVAL;

	res = dir_get(path, &dep);
	if (res == 0) {
		res = -EEXIST;
		goto err;
	} else if (res != -ENOENT) {
		goto err;
	}

	strcpy(buf, path);
	dir_basepath(buf);

	if ((res = dir_get(buf, &ddep)) < 0)
		return res;

	if (ddep->sp->flags & M_RO) {
		res = -EROFS;
		goto err;
	}

	if (!ddep->sp->fsdp->op->mkdir) {
		res = -EPERM;
		goto err;
	}

	if ((res = inode_get(ddep->sp, ddep->inum, &dp)) < 0)
		goto err;

	strcpy(buf, path);
	dir_basename(buf);

	if (!(dep = kmalloc(sizeof(struct dirent)))) {
		res = -ENOMEM;
		goto err;
	}

	list_init(&dep->l);
	dep->refs = 1;

	dep->inum = 0;
	strcpy(dep->name, buf);

	dep->sp = dp->sp;
	dep->pdep = ddep;

	if ((res = ddep->sp->fsdp->op->mkdir(dp, dep, mode | I_DIR)) < 0)
		goto err;

	dir_put(dep);
	inode_put(dp);
	dir_put(ddep);

	return 0;

err:
	if (dep)
		dir_put(dep);
	if (dp)
		inode_put(dp);
	if (ddep)
		dir_put(ddep);

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

void fs_unreg(struct fs_driver *fsdp)
{
	list_rm(&fsdp->l);
}

void fs_init(void)
{
	char buf[PATH_MAX + 1];
	dev_t root_dev;
	int res;

	if (cmdline_str_get("root", buf) != 0)
		goto fallback;
	dir_basename(buf);
	if (!(root_dev = device_getbyname(buf)))
		goto fallback;

	if ((res = cmdline_str_get("rootfs", buf)) != 0) {
		dprintf(devname, KP_ERR
				"missing/invalid cmdline parameter: rootfs (%d)"
				, res);
		goto fallback;
	}
	if ((res = fs_mount(root_dev, "/", buf, 0)) < 0) {
		dprintf(devname, KP_ERR "failed to mount root (%d)", res);
		goto fallback;
	}

	goto mountdev;

fallback:
	if ((res = fs_mount(DEV(MAJOR_MEM, MINOR_MEM_ROOT),
			"/", "memfs", 0)) < 0)
		panic("failed to mount fallback root", res, 0);

mountdev:
	res = fs_mkdir("/sys", 0);
	if (res == 0 || res == -EEXIST) {
		res = fs_mkdir("/sys/dev", 0);
		if (res < 0 && res != -EEXIST)
			panic("failed to create /sys/dev", res, 0);
	} else {
		panic("failed to create /sys", res, 0);
	}

	if ((res = fs_mount(DEV(MAJOR_MEM, MINOR_MEM_DEV),
			"/sys/dev", "devfs", 0)) < 0)
		panic("dev (devfs) failed to mount on /sys/dev", res, 0);

	return;
}
