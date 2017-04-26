/*
 *
 * Elarix
 * src/kernel/include/block.h
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

#ifndef _BLOCK_H
#define _BLOCK_H

#include <limits.h>
#include <list.h>

/* Block cache flags */
#define B_KEEP 01	/* Keep block cached */

struct block {
	struct list_head l;
	int refs;

	off_t	block;			/* Block number */
	char	buffer[BLOCK_SIZE];	/* Block number */
	u8	flags;			/* Block cache flags */
};

#endif
