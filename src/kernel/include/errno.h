/*
 *
 * Elarix
 * src/kernel/include/errno.h
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

#ifndef _ERRNO_H
#define _ERRNO_H

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
#define ENOSPC		28	/* No space left on device */
#define EROFS		30	/* Read-only file system */

#endif
