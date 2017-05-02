/*
 *
 * Elarix
 * src/kernel/dev/block/ide.h
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

#ifndef _IDE_H
#define _IDE_H

#include <list.h>

#define IDE_ATA		0x00	/* ATA device */
#define IDE_ATAPI	0x01	/* ATAPI device */

#define IDE_CHANNELS	2	/* Number of IDE channels */
#define IDE_DRIVES	2	/* Number of drives per IDE channel */

#define IDE_CH0_CMD		0x3F6
#define IDE_CH0_IO		0x1F0
#define IDE_CH1_CMD		0x376
#define IDE_CH1_IO		0x170

struct ata_ident {
	u16		type;
	u16		cylinders;
	u8		unused0[2];
	u16		heads;
	u8		unused1[4];
	u16		sectors_per_track;
	u16		vendor[3];
	char		serial[20];
	u8		unused2[6];
	char		revision[8];
	char		model[40];
	u8		unused3[4];
	struct {
		u8	unused4;
		u8	dma:1;
		u8	lba28:1;
		u8	unused5:6;
		u8	unused6[2];
	} __attribute__ ((packed)) features;
	u8		unused7[18];
	u32		lba28_max;
	u8		unused8[36];
	u16		revision_major;
	u16		revision_minor;
	struct {
		u8	unused9[3];
		u8	unused10:2;
		u8	lba48:1;
		u8	unused11:5;
		u8	unused12[2];
	} __attribute__ ((packed)) cmdset;
	struct {
		u8	unused13[3];
		u8	unused14:2;
		u8	lba48:1;
		u8	unused15:5;
		u8	unused16[2];
	} __attribute__ ((packed)) cmdset_active;
	u8		unused17[24];
	u64		lba48_max;
	u8		unused18[304];
} __attribute__ ((packed));

struct ide_device {
	struct list_head l;

	u16	base, ctrl, bus_master;
	u8	nint;

	u8	drive, type;
	u64	size;

	struct ata_ident ident;
};

#endif
