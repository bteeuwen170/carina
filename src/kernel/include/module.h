/*
 *
 * Elarix
 * src/kernel/include/module.h
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

#ifndef _MODULE_H
#define _MODULE_H

#define MODULE(n, i, e)

int ide_init(void);
void ide_exit(void);

int cmos_init(void);
void cmos_exit(void);

int ps2kbd_init(void);
void ps2kbd_exit(void);

/* int serial_init(void);
void serial_exit(void); */

int tty_init(void);
void tty_exit(void);

int pci_init(void);
void pci_exit(void);

int ac97_init(void);
void ac97_exit(void);

/* pcspk */

int sb16_init(void);
void sb16_exit(void);

int vga_init(void);
void vga_exit(void);

/* int devfs_init(void);
void devfs_exit(void);

int ext2_init(void);
void ext2_exit(void);

int iso9660_init(void);
void iso9660_exit(void); */

int ramfs_init(void);
void ramfs_exit(void);

#endif