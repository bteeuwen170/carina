/*
 *
 * Elarix
 * src/kernel/fs/fs.c
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
	(void) name;
	struct superblock *sp;

	sp = sb_alloc(driver);

	/* TEMP */ root_sb = list_entry(fs_drivers.next, struct fs_driver, l)->read_sb(sp);
	/* TODO */

	return NULL;
}

/* TODO Move to proc/ */
/* int sys_chdir(const char *path)
{
	//TODO
} */

/* TODO Move to ipc/ */
/* int sys_pipe(int fd[2])
{
	//TODO
} */

int sys_open(const char *path, int flags, mode_t mode)
{
	(void) flags;
	struct dirent *dep;
	struct file *fp;
	int fd = -1;

	dep = dirent_get(path);
	if (!dep)
		goto err;

	fp = file_alloc(dep);
	if (!fp)
		goto err;

	fp->mode = mode;

	fd = fd_alloc(fp);
	if (fd < 0)
		goto err;
	//open_namei 0.99 namei.c <- from <- open.c (sys_open)

	return fd;

err:
	file_dealloc(fp);
	/* fd_dealloc(fd); */

	return fd;
}

/* int sys_close(int fd)
{
	//TODO
} */

/* int sys_read(int fd, char *buf, size_t n)
{
	//TODO
} */

/* int sys_write(int fd, const char *buf, size_t n)
{
	//TODO
} */

int sys_readdir(int fd, struct usr_dirent *udep)
{
	struct file *fp;
	struct usr_dirent *ludep;

	fp = file_get(fd);
	if (!fp)
		return -EBADF;

	ludep = usr_dirent_get(fp);
	if (!ludep)
		return -1; //TODO

	/* TODO Update atime */
	/* TODO Loop */
	/* TODO memcpy to userspace */
	memcpy(udep, ludep, sizeof(struct usr_dirent));

	return 1;
}

/* int sys_link(const char *oldpath, const char *path)
{
	//TODO
} */

/* int sys_symlink(const char *oldpath, const char *path)
{
	//TODO
} */

/* int sys_rmlink(const char *path)
{
	//TODO
} */

int sys_mkdir(const char *path, mode_t mode)
{
	(void) path, (void) mode;
	//TODO
}

/* int sys_rmdir(const char *path)
{
	//TODO
} */

/* int sys_mknod(const char *path, mode_t mode, dev_t dev)
{
	//TODO
} */

/* int sys_move(const char *oldpath, const char *path)
{
	//TODO
} */

/* int sys_stat(int fd, struct stat *buf)
{
	//TODO
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
