/*
 *
 * Elarix
 * src/kernel/dev/char/pty.c
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
#include <kernel.h>
#include <module.h>
#include <tty.h>

static const char devname[] = "pty";

static int pty_open(struct tty *tty, struct file *fp)
{

}

static int pty_close(struct tty *tty, struct file *fp)
{

}

static int tty_write(struct tty *tty, const void *buf, off_t off, size_t n)
{

}

static struct file_ops pty_file_ops = {
	.open	= &tty_open,
	.close	= &tty_close,
	.write	= &tty_write
};

int tty_init(void)
{
	int res;

	res = dev_reg(0, devname, &tty_file_ops);
	if (res < 0)
		panic("%s: unable to register tty (%d)", devname, res);

	return 0;
}

void tty_exit(void)
{
	
	/* TODO */
}

MODULE(pty, &pty_init, &pty_exit);
