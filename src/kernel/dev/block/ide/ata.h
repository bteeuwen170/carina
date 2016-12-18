/*
 *
 * Elara
 * src/kernel/dev/block/ide/ata.h
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

#ifndef _IDE_ATA_H
#define _IDE_ATA_H

#define ATA_CHANNELS		2
#define ATA_DRIVES		2

#define ATA_SECTOR_SIZE		512
#define ATAPI_SECTOR_SIZE	2048


#define ATA_REG_IO		0x00	/* I/O */
#define ATA_REG_ERROR		0x01	/* Error */
#define ATA_REG_FEATURES	0x01	/* Features */
#define ATA_REG_SECTORS0	0x02	/* Sectors low */
#define ATA_REG_LBA0		0x03	/* LBA 0 low */
#define ATA_REG_LBA1		0x04	/* LBA 0 middle */
#define ATA_REG_LBA2		0x05	/* LBA 0 high */
#define ATA_REG_DRVSEL		0x06	/* Drive selector */
#define ATA_REG_CMD		0x07	/* Command */
#define ATA_REG_STATUS		0x07	/* Status */
#define ATA_REG_SECTORS1	0x08	/* Sectors high */
#define ATA_REG_LBA3		0x09	/* LBA 1 low */
#define ATA_REG_LBA4		0x0A	/* LBA 1 middle */
#define ATA_REG_LBA5		0x0B	/* LBA 1 high */

#define ATA_REG_CTRL		0x0C	/* Control */
#define ATA_REG_STATUS_ALT	0x0C	/* Alternative status */

#define ATA_CH0_CMD		0x3F6
#define ATA_CH0_IO		0x1F0
#define ATA_CH1_CMD		0x376
#define ATA_CH1_IO		0x170

#define ATA_CMD_BUSY		0x80	/* Busy */
#define ATA_CMD_READY		0x40	/* Ready */
#define ATA_CMD_WERR		0x20	/* Write error */
#define ATA_CMD_DSEEK		0x10	/* Seek done */
#define ATA_CMD_DDR		0x08	/* Data request done */
#define ATA_CMD_COR		0x04	/* Data corrected */
#define ATA_CMD_INDEX		0x02	/* Index */
#define ATA_CMD_ERR		0x01	/* Error */

#define ATA_ERR_SBAD		0x80	/* Bad sector */
#define ATA_ERR_DUNC		0x40	/* Data uncorrectable */
#define ATA_ERR_NM0		0x20	/* No media */
#define ATA_ERR_DSEEK		0x10	/* No ID mark */
#define ATA_ERR_NM1		0x08	/* No media */
#define ATA_ERR_CABRT		0x04	/* Command aborted */
#define ATA_ERR_NTRK0		0x02	/* Track 0 missing */
#define ATA_ERR_NADRM		0x01	/* No address mark */

#define ATA_CMD_PIO_READ28	0x20	/* ATA PIO read LBA 28 */
//#define ATA_CMD_PIO_READ48	0x24	/* ATA PIO read LBA 48 */
#define ATA_CMD_PIO_WRITE28	0x30	/* ATA PIO write LBA 28 */
//#define ATA_CMD_PIO_WRITE48	0x34	/* ATA PIO write LBA 48 */
//#define ATA_CMD_DMA_READ28	0xC8	/* ATA DMA read LBA 28 */
//#define ATA_CMD_DMA_READ48	0x25	/* ATA DMA read LBA 48 */
//#define ATA_CMD_DMA_WRITE28	0xCA	/* ATA DMA write LBA 28 */
//#define ATA_CMD_DMA_WRITE48	0x35	/* ATA DMA write LBA 48 */
#define ATA_CMD_PACKET		0xA0	/* ATA packet */
#define ATA_CMD_IDENT		0xEC	/* ATA identify */

#define ATAPI_CMD_READ		0xA8	/* ATAPI read */
#define ATAPI_CMD_EJECT		0x1B	/* ATAPI eject */
#define ATAPI_CMD_IDENT		0xA1	/* ATAPI identify */

/*#define ATA_IDENT_TYPE		0x00
#define ATA_IDENT_CYLS		0x02
#define ATA_IDENT_HEADS		0x06
#define ATA_IDENT_SECTORS	0x0C
#define ATA_IDENT_SERIAL	0x14
#define ATA_IDENT_MODEL		0x36
#define ATA_IDENT_FEATURES	0x62
#define ATA_IDENT_FV		0x6A
#define ATA_IDENT_MAX_LBA	0x78
#define ATA_IDENT_CMDSETS	0xA4*/

struct ata_ident {
	u16		type;
	u16		cylinders;
	u8		reserved0[2];
	u16		heads;
	u8		unused0[4];
	u16		sectors_per_track;
	u16		vendor[3];
	char		serial[20];
	u8		unused1[6];
	char		revision[8];
	char		model[40];
	u8		unused2[2];
	u8		reserved1[2];
	struct {
		u8	reserved0;
		u8	dma:1;
		u8	lba28:1;
		u8	unused0:2;
		u8	reserved1:1;
		u8	unused1:1;
		u8	reserved2:2;
		u8	reserved3[2];
	} features;
	u8		unused3[4];
	u8		reserved2[2];
	u8		unused4[12];
	u32		lba28_max;
	u8		unused5[14];
	u8		reserved3[22];
	u16		revision_major;
	u16		revision_minor;
	struct {
		u8	unused0[3];
		u8	unused1:2;
		u8	lba48:1;
		u8	unused2[2];
		u8	unused3:5;
	} cmdset;
	struct {
		u8	unused0[3];
		u8	unused1:2;
		u8	lba48:1;
		u8	unused2[2];
		u8	unused3:5;
	} cmdset_active;
	u8		udma_support;
	u8		udma_active;
	u8		reserved4[8];
	u8		unused6[14];
	u64		lba48_max;
	/* TODO Finish */
} __attribute__ ((packed));

#define ATA_DEV_TYPE_ATA	0x00	/* ATA device */
#define ATA_DEV_TYPE_ATAPI	0x01	/* ATAPI device */

struct ata_dev {
	u8			ch;
	u8			drv;
	u8			type;
	u64			size;

	struct ata_ident	ident;
};

struct ata_channel {
	u16	base;
	u16	ctrl;
	u16	bus_master;
	u8	nint;
};

void ide_init(void);

#endif
