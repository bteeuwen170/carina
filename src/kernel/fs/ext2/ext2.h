/*
 *
 * Clemence
 * src/kernel/fs/ext2/ext2.h
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

#ifndef _EXT2_H
#define _EXT2_H

//TODO Down here ...

#define EXT2_BLOCKS 15

/*#define EXT2_STATE_CLEAN	1
#define EXT2_STATE_ERRORS	2

#define EXT2_ERR_IGNORE		1
#define EXT2_ERR_REMOUNT_RO	2
#define EXT2_KERNEL_PANIC	3

#define EXT2_CREATED_LINUX	0
#define EXT2_CREATED_GNU_HURD	1
#define EXT2_CREATED_MASIX	2
#define EXT2_CREATED_FREEBSD	3
#define EXT2_CREATED_LITE	4

#define EXT2_PREALLOC_BLOCKS	0x01
#define EXT2_AFS_INODES		0x02
#define EXT2_JOURNAL		0x04
#define EXT2_EXTENDED_INODES	0x08
#define EXT2_RESIZABLE		0x10
#define EXT2_HASH_INDEX		0x20

#define EXT2_COMPRESSED		0x01
#define EXT2_DIR_TYPE		0x02
#define EXT2_REPLAY_JOURNAL	0x04
#define EXT2_JOURNAL_DEVICE	0x08

#define EXT2_SPARSE_SUPERBLOCKS	0x01
#define EXT2_64BIT_FILESIZE	0x02
#define EXT2_DIR_BINTREE	0x04

#define EXT2_SUPERBLOCK_START	1024
#define EXT2_SUPERBLOCK_LENGTH	1024*/

/* File types */
#define EXT2_TYPE_PIPE		0x1000
#define EXT2_TYPE_CHARDEV	0x2000
#define EXT2_TYPE_DIR		0x4000
#define EXT2_TYPE_BLOCKDEV	0x6000
#define EXT2_TYPE_FILE		0x8000
#define EXT2_TYPE_LINK		0xA000
#define EXT2_TYPE_SOCKET	0xC000

/* Permissions */
#define EXT2_TYPE_UR	0x0100		/* Owner read */
#define EXT2_TYPE_UW	0x0080		/* Owner write */
#define EXT2_TYPE_UE	0x0040		/* Owner execute */
#define EXT2_TYPE_GR	0x0020		/* Group read */
#define EXT2_TYPE_GW	0x0010		/* Group write */
#define EXT2_TYPE_GE	0x0008		/* Group execute */
#define EXT2_TYPE_OR	0x0004		/* Other read */
#define EXT2_TYPE_OW	0x0002		/* Other write */
#define EXT2_TYPE_OE	0x0001		/* Other execute */
//#define EXT2_TYPE_SUID	0x0001	/* Set UID on execution */
//#define EXT2_TYPE_SGID	0x0001	/* Set GID on execution */

/* Flags */
#define EXT2_FLAGS_SYNC		0x0008	/* Synchronous updates */
#define EXT2_FLAGS_IMMUTABLE	0x0010	/* Immutable */
#define EXT2_FLAGS_AO		0x0020	/* Appending only */
#define EXT2_FLAGS_NOATIME	0x0080	/* Do not update access time */

struct ext2_sb {
	u32	inodes;			/* Inodes */
	u32	blocks;			/* Blocks */

	u32	blocks_reserved;	/* Reserved blocks */
	u32	blocks_free;		/* Free blocks */
	u32	inodes_free;		/* Free inodes */
	u32	block_offset;		/* First data block offset */
	u32	block_size;		/* Block size */

	u32	fragment_size;		/* Fragment size */
	u32	blocks_bg;		/* Blocks per block group */
	u32	fragments_bg;		/* Fragments per block group */
	u32	inodes_bg;		/* Inodes per block group */
	u8	reserved0[12];

	u16	signature;		/* 0xEF53 */

	u8	reserved1[70];
} __attribute__ ((packed));

struct ext2_bg_desc {
	u32	bitmap_block;		/* Blocks bitmap block */
	u32	inode_block;		/* Inodes bitmap block */
	u32	inode_tbl_block;	/* Inodes table block */
	u16	blocks_free;		/* Free blocks */
	u16	inodes_free;		/* Free inodes */
	u16	dirs;			/* Directories */
	u8	reserved0[14];
} __attribute__ ((packed));

struct ext2_inode {
	u16	type;
	u16	uid;
	u32	size;
	u32	atime;
	u32	ctime;
	u32	mtime;
	u8	unused0[4];
	u16	gid;
	u16	links;
	u32	sectors;
	u32	flags;			/* File flags */
	u8	unused1[4];
	u32	block[EXT2_BLOCKS];	/* Pointer to blocks */
	u8	unused2[28];
} __attribute__ ((packed));

struct ext2_inode_info {
	u32	block[EXT2_BLOCKS];
	u32	flags;
};

struct ext2_dir_entry {
	u32	inode;
	u16	size;
	u8	name_len;
	u8	type;
	char name[];
} __attribute__ ((packed));

void ext2_init(void);

#endif
