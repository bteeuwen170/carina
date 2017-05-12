/*
 *
 * Elarix
 * src/kernel/dev/block/ata.c
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

#include <ata.h>
#include <dev.h>
#include <fs.h>
#include <kernel.h>
#include <module.h>
#include <pci.h>

static const char devname[] = "ata";

int ata_probe(struct device *devp)
{
	return -1;
}

static int ata_read(struct file *fp, char *buf, off_t off, size_t n)
{
	return -1;
}

static int ata_write(struct file *fp, const char *buf, off_t off, size_t n)
{
	return -1;
}

static struct file_ops ata_file_ops = {
	.read	= &ata_read,
	.write	= &ata_write
};

int ata_init(void)
{
	return 0;
}

void ata_exit(void) { }

MODULE(ata, &ata_init, &ata_exit);
MODULE_DEP(ide);
