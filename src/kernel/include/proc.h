/*
 *
 * Elarix
 * src/kernel/include/proc.h
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

#ifndef _PROC_H
#define _PROC_H

#include <fs.h>

struct process {
	pid_t	pid;

	uid_t	uid;
	gid_t	gid;

	/* char **argv, **envp; */

	struct file	*fd[FD_MAX];
	/* struct process	*parent; */
	struct dirent	*cwd;
};

extern struct process *cproc;

int proc_exec(const char *path, char **argv, char **envp);

void proc_init(void);

#endif
