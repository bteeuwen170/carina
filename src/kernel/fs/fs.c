/*
 *
 * Elarix
 * src/kernel/fs/fs.c
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

/* TODO Clean includes */
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <limits.h>
#include <kernel.h>
#include <sys/types.h> /* XXX TEMP for syntax highlighting */

#include <stdlib.h>
#include <string.h>

static const char devname[] = "fs";

static LIST_HEAD(fs_drivers);

struct superblock *root_sb;

struct mountp *sv_mount(struct fs_driver *driver, const char *name)
{
	struct superblock *sp;
	(void) name;

	sp = sb_alloc(driver);

	/* TEMP */ root_sb = list_entry(fs_drivers.next, struct fs_driver, l)->read_sb(sp);
	/* TODO */

	return NULL;
}

int sv_mkdir(const char *path, mode_t mode)
{
	/* XXX TEMP? */ return sys_mkdir(path, mode);
}

/* TODO Move to proc/ */
/* int sys_chdir(const char *path)
{
} */

/* TODO Move to ipc/ */
/* int sys_pipe(int fd[2])
{
} */

int sys_open(const char *path, int flags, mode_t mode)
{
	struct dirent *dep = NULL;
	struct file *fp = NULL;
	int res = -1;
	(void) flags;

	if (!(dep = dirent_get(path))) {
		res = -ENOENT;
		goto err;
	}

	if (!(fp = file_alloc(dep))) {
		res = -EMFILE; /* FIXME Right? */
		goto err;
	}

	fp->mode = mode;

	if ((res = fd_alloc(fp)) < 0)
		goto err;
	/* open_namei 0.99 namei.c <- from <- open.c (sys_open) */

	return res;

err:
	if (fp)
		file_put(fp);
	if (dep)
		dirent_put(dep);

	return res;
}

/* int sys_close(int fd)
{
} */

/* int sys_read(int fd, char *buf, size_t n)
{
} */

int sys_write(int fd, const char *buf, size_t n)
{
	/* XXX FIXME TEMP */
	struct file *fp;

	if (!(fp = file_get(fd)))
		return -EBADF;

	fp->dep->ip->fop->write(fp, buf, 0, n);
}

int sys_readdir(int fd, struct usr_dirent *udep)
{
	struct file *fp;
	struct usr_dirent *ludep;
	int res = -1;

	if (!(fp = file_get(fd))) {
		res = -EBADF;
		goto err;
	}

	if (!(fp->dep->ip->mode & IM_DIR)) {
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

	if (!(dep->ip = inode_alloc(dp->ip->sp))) {
		res = -ENOMEM;
		goto err;
	}
	dep->ip->mode = IM_DIR | mode;

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
