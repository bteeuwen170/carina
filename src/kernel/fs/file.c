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
	/* TODO Handle O_CREATE */

	if (cfp->dep->sp->flags & M_RO && !(mode & O_RO)) {
		res = -EROFS;
		goto err;
	}

	if ((res = inode_get(cfp->dep->sp, cfp->dep->inum, &cfp->dp)) < 0)
		goto err;

	if (!(cfp->dp->mode & I_DIR) && mode & O_DIR) {
		res = -ENOTDIR;
		goto err;
	}

	*fp = cfp;

	return 0;

err:
	if (cfp->dp)
		inode_put(cfp->dp);
	if (cfp->dep)
		dir_put(cfp->dep);
	if (cfp)
		kfree(cfp);

	return res;
}

int file_close(struct file *fp)
{
	/* TODO */
	return 0;
}

/* int file_read(struct file *fp, char *buf, off_t off, size_t n)
{
	if (!fp)
		return -EBADF;

	if (!n)
		return 0;
} */

int file_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	if (!fp)
		return -EBADF;

	if (!n)
		return 0;

	/* TODO */

	return fp->dp->fop->write(fp, buf, off, n);
}

int file_readdir(struct file *fp, char *_name)
{
	if (!fp)
		return -EBADF;

	if (!(fp->dp->mode & I_DIR))
		return -ENOTDIR;

	return fp->dp->fop->readdir(fp, _name);
}

/* int file_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	if (!fp)
		return -EBADF;
} */

/* int file_stat(struct file *fp. struct stat *sp)
{
	if (!fp)
		return -EBADF;
} */
