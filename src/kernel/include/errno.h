/*
 *
 * Elarix
 * src/kernel/include/errno.h
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

#ifndef _ERRNO_H
#define _ERRNO_H

#include <limits.h>

#define EPERM		1	/* Operation not permitted */
#define ENOENT		2	/* No such file or directory */
#define EBADF		9	/* Invalid file descriptor */
#define ENOMEM		12	/* Out of memory */
#define EACCES		13	/* Permission denied */
#define EBUSY		16	/* Device or resource busy */
#define EEXIST		17	/* File exists */
#define ENODEV		19	/* No such device */
#define ENOTDIR		20	/* Not a directory */
#define EINVAL		22	/* Invalid argument */
#define EMFILE		24	/* Too many open files */
#define ENOSPC		28	/* No space left on device */
#define EROFS		30	/* Read-only file system */

/* static inline void *__attribute__((warn_unused_result)) ERR_PTR(intptr_t errno)
{
	return (void *) errno;
}

static inline intptr_t __attribute__((warn_unused_result)) PTR_ERR(const void *ptr)
{
	return (intptr_t) (void *) ptr;
}

static inline int __attribute__((warn_unused_result)) IS_ERR(const void *ptr)
{
	return (PTR_ERR(ptr) < 0) && (PTR_ERR(ptr) > -(ERRNO_MAX));
} */

#endif
