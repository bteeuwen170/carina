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

#define EACCES		1	/* Permission denied */
#define EBADF		2	/* Invalid file descriptor */
#define EBUSY		3	/* Device or resource busy */
#define EEXIST		4	/* File exists */
#define EFAULT		5	/* Invalid address */
#define EINVAL		6	/* Invalid argument */
#define EIO		7	/* I/O error */
#define EMFILE		8	/* Too many open files */
#define ENAMETOOLONG	9	/* File name too long */
#define ENODEV		10	/* No such device */
#define ENOENT		11	/* No such file or directory */
#define ENOMEM		12	/* Out of memory */
#define ENOSPC		13	/* No space left on device */
#define ENOTDIR		14	/* Not a directory */
#define EPERM		15	/* Operation not permitted */
#define EROFS		16	/* Read-only file system */

#endif
