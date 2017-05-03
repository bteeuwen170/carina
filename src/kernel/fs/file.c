/*
 *
 * Elarix
 * src/kernel/fs/file.c
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

#include <string.h>
#include <stdlib.h>

int file_open(const char *path, mode_t mode, struct file **fp)
{
	struct file *cfp;
	int res;

	if (strlen(path) > PATH_MAX)
		return -ENAMETOOLONG;

	if (!(cfp = kcalloc(1, sizeof(struct file))))
		return -ENOMEM;

	cfp->off = 0;

	if ((res = dir_get(path, &cfp->dep)) < 0)
		goto err;
	/* TODO Handle F_CREATE */

	if ((res = inode_get(cfp->dep->sp, cfp->dep->inum, &cfp->ip)) < 0)
		goto err;

	if (!cfp->ip->op) {
		res = -EPERM;
	}

	if (cfp->ip->sp->flags & M_RO && !(mode & F_RO)) {
		res = -EROFS;
		goto err;
	}

	if (!(cfp->ip->mode & I_DIR) && mode & F_DIR) {
		res = -ENOTDIR;
		goto err;
	}

	cfp->mode = mode;

	*fp = cfp;

	return 0;

err:
	if (cfp->ip)
		inode_put(cfp->ip);
	if (cfp->dep)
		dir_put(cfp->dep);
	if (cfp)
		kfree(cfp);

	return res;
}

void file_close(struct file *fp)
{
	if (!fp)
		return;

	inode_put(fp->ip);
	dir_put(fp->dep);

	kfree(fp);
}

int file_read(struct file *fp, char *buf, size_t n)
{
	if (!fp || fp->mode & F_DIR)
		return -EBADF;

	if (!fp->ip->op->read)
		return -EPERM;

	if (!n)
		return 0;

	/* TODO */

	return fp->ip->op->read(fp, buf, 0, n);
}

int file_write(struct file *fp, const char *buf, size_t n)
{
	if (!fp || fp->mode & F_RO || fp->mode & F_DIR)
		return -EBADF;

	if (!fp->ip->op->write)
		return -EPERM;

	if (!n)
		return 0;

	/* TODO */

	return fp->ip->op->write(fp, buf, 0, n);
}

int file_readdir(struct file *fp, char *_name)
{
	if (!fp)
		return -EBADF;

	if (!(fp->mode & F_DIR))
		return -ENOTDIR;

	if (!fp->ip->op->readdir)
		return -EPERM;

	return fp->ip->op->readdir(fp, _name);
}

/* int file_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	if (!fp)
		return -EBADF;

	if (!fp->ip->op->ioctl)
		return -EPERM;
} */

/* int file_stat(struct file *fp. struct stat *sp)
{
	if (!fp)
		return -EBADF;

	if (!fp->ip->op->stat)
		return -EPERM;
} */
