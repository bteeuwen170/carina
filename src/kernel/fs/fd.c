/*
 *
 * Elarix
 * src/kernel/fs/fd.c
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

int fd_alloc(struct file *fp)
{
	int fd;

	for (fd = 0; fd < FD_MAX; fd++) {
		if (cproc->fd[fd])
			continue;

		cproc->fd[fd] = fp;

		return fd;
	}

	return -EMFILE;
}

void fd_dealloc(int fd)
{
	cproc->fd[fd] = 0;
}
