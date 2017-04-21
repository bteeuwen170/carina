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

/* TODO Clean includes */
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <kernel.h>
#include <limits.h>
#include <list.h>
#include <proc.h>
#include <sys/types.h> /* XXX TEMP for syntax highlighting */

#include <stdlib.h>
#include <string.h>

static const char devname[] = "fs";

static LIST_HEAD(fs_drivers);

struct superblock *root_sb;
struct dirent root_dep;

int sv_mkdir(const char *path, mode_t mode)
{
	/* XXX TEMP? */ return sys_mkdir(path, mode);
}

int sys_mount(const char *device, const char *path, const char *fs)
{
	struct fs_driver *driver;
	struct superblock *sp = NULL;
	struct dirent *dep;
	int res = -1;

	list_for_each(driver, &fs_drivers, l)
		if (strcmp(driver->name, fs) == 0)
			goto foundfs;

	return -EINVAL;

foundfs:
	if (!(sp = sb_alloc(driver)))
		return -ENOMEM;

	if (device == NULL && strcmp(path, "/") == 0) {
		if (root_sb) {
			res = -EINVAL;
			goto err;
		}

		root_sb = sp;

		list_init(&root_dep.l);

		root_dep.name[0] = '/';
		root_dep.name[1] = '\0';
		root_dep.type = DT_DIR;

		root_dep.refs = 1;

		root_dep.ip = driver->read_sb(sp);
		cproc->cwd = root_dep.dp = &root_dep;

		root_dep.ip->mode = IM_UM | IM_GR | IM_GE | IM_OR | IM_OE;

		root_dep.ip->uid = root_dep.ip->gid = 0;

		if (!(dep = dirent_alloc(&root_dep, "."))) {
			res = -ENOMEM;
			goto err;
		}
		dep->ip = root_dep.ip;

		if (!(dep = dirent_alloc(&root_dep, ".."))) {
			res = -ENOMEM;
			goto err;
		}
		dep->ip = root_dep.ip;
	} else {
		if (!(dep = dirent_get(device))) {
			res = -ENOENT;
			goto err;
		}

		/* TODO Error checking */
		sp->dev = dep->ip->dev;

		/* TODO Check if dir is empty */
		if (!(dep = dirent_get(path))) {
			res = -ENOENT;
			goto err;
		}

		inode_put(dep->ip);

		dep->ip = driver->read_sb(sp);

		dep->ip->op->readdir(dep);
	}

	dprintf(devname, "successfully mounted %s (%s) on %s\n",
			device, dep->ip->sp->name, path);

	return 0;

err:
	/* if (sp)
		sb_dealloc(sp); */

	return res;
}

/* TODO Move to proc/ */
int sys_chdir(const char *path)
{
	struct dirent *dep;
	int res = -1;

	if (!(dep = dirent_get(path))) {
		res = -ENOENT;
		goto err;
	}

	if (!(dep->type & DT_DIR)) {
		res = -ENOTDIR;
		goto err;
	}

	cproc->cwd = dep;

	return 0;

err:
	if (dep)
		dirent_put(dep);

	return res;
}

int sys_cwdir(char *path)
{
	struct dirent *dep = cproc->cwd;
	char buf[PATH_MAX];
	char *pb = buf, *nb = buf;

	buf[0] = '\0';

	do {
		strcat(buf, dep->name);
		if (dep != &root_dep)
			strcat(buf, "/");

		dep = dep->dp;
	} while (dep != &root_dep);

	while (*nb++) {
		if (*nb == '\0') {
			strnrev(pb, nb - pb);
		} else if (*nb == '/') {
			strnrev(pb, nb - pb);
			pb = nb + 1;
		}
	}

	strrev(buf);

	strcpy(path, buf);

	return 0;
}

/* TODO Move to ipc/ */
/* int sys_pipe(int fd[2])
{
} */

struct file *fs_open(const char *path, int flags, mode_t mode)
{
	struct dirent *dep = NULL;
	struct file *fp = NULL;
	(void) flags;

	if (!(dep = dirent_get(path)))
		goto err;

	if (!(fp = file_alloc(dep)))
		goto err;

	/* dep->ip->fop->open(dep->ip, fp); */
	/* open_namei 0.99 namei.c <- from <- open.c (sys_open) */

	return fp;

err:
	if (fp)
		file_put(fp);
	if (dep)
		dirent_put(dep);

	return NULL;
}

int fs_close(struct file *fp)
{
	/* dep->ip->fop->close(dep->ip, fp); */

	file_put(fp);

	return 0;
}

int fs_read(struct file *fp, char *buf, off_t off, size_t n)
{
	return fp->dep->ip->fop->read(fp, buf, off, n);
}

int fs_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	return fp->dep->ip->fop->write(fp, buf, off, n);
}

int fs_readdir(struct file *fp, struct usr_dirent *udep)
{
	struct usr_dirent *ludep;
	int res = -1;

	if (!(fp->dep->type & DT_DIR)) {
		res = -ENOTDIR;
		goto err;
	}

	if (!(ludep = usr_dirent_get(fp)))
		return 0;

	/* TODO Update atime */
	/* TODO Loop */
	/* TODO memcpy to userspace */
	memcpy(udep, ludep, sizeof(struct usr_dirent));

	return 1;

err:
	/* if (ludep)
		kfree(ludep); */

	return res;
}

/* int sys_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{

} */

/* int sys_link(const char *oldpath, const char *path)
{
} */

/* int sys_symlink(const char *oldpath, const char *path)
{
} */

/* int sys_rmlink(const char *path)
{
} */

int sys_mkdir(const char *path, mode_t mode)
{
	struct dirent *dp = NULL, *dep = NULL, *cdep = NULL, *pdep = NULL;
	const char *i, *n;
	char p[PATH_MAX + 1];
	int j, res = -1;

	for (i = path, n = path, p[0] = '\0', j = 0; *i; i++, j++) {
		if (*i == '/' && *(i + 1)) {
			strncpy(p, path, j + 1);
			p[j + 1] = '\0';

			n = i + 1;
		}
	}

	if (!(dp = dirent_get(p))) {
		res = -ENOENT;
		goto err;
	}

	if (!(dep = dirent_alloc(dp, n))) {
		res = -1; /* TODO */
		goto err;
	}
	dep->type = DT_DIR;

	if (!(dep->ip = inode_alloc(dp->ip->sp))) {
		res = -ENOMEM;
		goto err;
	}
	dep->ip->mode |= mode;
	dep->ip->op = dp->ip->op;

	if (!(cdep = dirent_alloc(dep, "."))) {
		res = -1; /* TODO */
		goto err;
	}
	cdep->ip = dep->ip;

	if (!(pdep = dirent_alloc(dep, ".."))) {
		res = -1; /* TODO */
		goto err;
	}
	pdep->ip = dep->ip;

	return 0;

err:
	if (pdep)
		dirent_put(pdep);
	if (cdep)
		dirent_put(cdep);
	if (dep && dep->ip)
		inode_put(dep->ip);
	if (dep)
		dirent_put(dep);
	if (dp)
		dirent_put(dp);

	return res;
}

/* int sys_rmdir(const char *path)
{
} */

/* int sys_mknod(const char *path, mode_t mode, dev_t dev)
{
} */

/* int sys_move(const char *oldpath, const char *path)
{
} */

/* int sys_stat(int fd, struct stat *buf)
{
} */

int fs_reg(struct fs_driver *driver)
{
	/* TODO Check if not already present */

	list_init(&driver->l);
	list_add(&fs_drivers, &driver->l);

	return 0;
}

void fs_unreg(struct fs_driver *driver)
{
	/* TODO Check for presence */

	list_rm(&driver->l);
}
