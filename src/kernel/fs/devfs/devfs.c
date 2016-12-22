/*
 *
 * Elarix
 * src/kernel/fs/devfs/devfs.c
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

#include <fs.h>

static const struct sb_ops devfs_sb_ops = {
	.alloc_inode = NULL,
	.dealloc_inode = NULL,
	.write_inode = NULL,
	.delete_inode = NULL
};

static const struct inode_ops devfs_inode_ops = {
	.symlink = NULL,
	.rmlink = NULL,
	.mkdir = NULL,
	.rmdir = NULL,
	.mknod = NULL,
	.move = NULL
};
