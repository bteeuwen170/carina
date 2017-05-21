/*
 *
 * Elarix
 * src/kernel/kernel/proc.c
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

#include <elf.h>
#include <errno.h>
#include <fs.h>
#include <mm.h>
#include <proc.h>

extern void usermode_enter();

static struct process _cproc;
struct process *cproc = &_cproc;

int proc_exec(const char *path, char **argv, char **envp)
{
	struct file *fp;
	char *buf;
	int res;

	if ((res = file_open(path, F_RO, &fp)) < 0)
		return res;

	if (!fp->ip->size)
		return -EINVAL;

	if (!(buf = kmalloc(fp->ip->size, 0)))
		return -ENOMEM;

	if ((res = file_read(fp, buf, fp->ip->size)) < 0)
		return res;

	if ((res = elf_read_header((struct elf_header *) buf)) == 0)
		return res;

	if (res == 1)
		res = elf32_load((struct elf_header64 *) buf);
	else if (res == 2)
		res = elf64_load((struct elf_header64 *) buf);

	return res;
}

void proc_init(void)
{

}
