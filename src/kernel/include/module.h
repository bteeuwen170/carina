/*
 *
 * Elarix
 * src/kernel/include/module.h
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

#ifndef _MODULE_H
#define _MODULE_H

#define MODULE(n, i, e)

#define MODULE_DEP(n)
#define MODULE_BEFORE(n)

#if 1
int ide_init(void);
void ide_exit(void);

int ata_init(void);
void ata_exit(void);

int atapi_init(void);
void atapi_exit(void);

int cmos_init(void);
void cmos_exit(void);

int ps2kbd_init(void);
void ps2kbd_exit(void);

/* int serial_init(void);
void serial_exit(void); */

int pci_init(void);
void pci_exit(void);

int ac97_init(void);
void ac97_exit(void);

/* pcspk */

int sb16_init(void);
void sb16_exit(void);

int serial_con_init(void);
void serial_con_exit(void);

int vga_con_init(void);
void vga_con_exit(void);

int iso9660_init(void);
void iso9660_exit(void);
#endif

#endif
