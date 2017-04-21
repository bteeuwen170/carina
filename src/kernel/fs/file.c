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
#include <limits.h>
#include <proc.h>

#include <stdlib.h>

int file_cnt = 0;

struct file *file_alloc(struct dirent *dep)
{
	struct file *fp;

	if (file_cnt + 1 > FD_MAX)
		return NULL;

	if (!(fp = kmalloc(sizeof(struct file))))
		return NULL;

	fp->mode = 0;
	fp->off = 0;

	fp->refs = 1;

	fp->dep = dep;

	file_cnt++;

	return fp;
}

static void file_dealloc(struct file *fp)
{
	if (!fp)
		return;

	kfree(fp);

	file_cnt--;
}

void file_put(struct file *fp)
{
	fp->refs--;

	if (!fp->refs) {
		file_dealloc(fp);
	}
}
