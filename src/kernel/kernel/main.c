/*
 *
 * Elarix
 * src/kernel/kernel/main.c
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

#include <cmdline.h>
#include <delay.h>
#include <dev.h>
#include <errno.h>
#include <fs.h>
#include <ioctl.h>
#include <issue.h>
#include <kbd.h>
#include <kernel.h>
#include <lock.h>
#include <module.h>
#include <pci.h>
#include <proc.h>
#include <reboot.h>
#include <time.h>

#include <sound/ac97.h>
#include <sound/sb16.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void kernel_main(void)
{
	struct tm tm;

	/* SPINLOCK(main); */
	/* spin_lock(main); */

#ifdef CONFIG_CONSOLE_SERIAL
	serial_con_init();
#endif
#ifdef CONFIG_CONSOLE_VGA
	vga_con_init();
#endif
#ifdef CONFIG_ISO9660
	iso9660_init();
#endif
#ifdef CONFIG_PIT
	pit_init();
#endif
#ifdef CONFIG_IDE
	ide_init();
#endif
#ifdef CONFIG_ATA
	ata_init();
#endif
#ifdef CONFIG_ATAPI
	atapi_init();
#endif
#ifdef CONFIG_CMOS
	cmos_init();
#endif
#ifdef CONFIG_PS2KBD
	ps2kbd_init();
#endif
#ifdef CONFIG_AC97
	ac97_init();
#endif
#ifdef CONFIG_SB16
	sb16_init();
#endif

	asm volatile ("sti");

	timer_init();
	rtc_init();
#ifdef CONFIG_PCI
	pci_init();
#endif

	devices_probe();

	memfs_init();
	devfs_init();
	fs_init();

#if 0
	/* Initialize video hardware properly */
	dprintf("fb", KP_DBG "addr is %#x\n", mboot->framebuffer_addr);
	dprintf("fb", KP_DBG "bpp is %#x\n", mboot->framebuffer_bpp);
	dprintf("fb", KP_DBG "pitch is %#x\n", mboot->framebuffer_pitch);
	dprintf("fb", KP_DBG "type is %#x\n", mboot->framebuffer_type);

	if (mboot->framebuffer_type == 1) {
		u64 *fb = (u64 *) (u64) mboot->framebuffer_addr;
		u32 color = ((1 << mboot->framebuffer_blue_mask_size) - 1) <<
				mboot->framebuffer_blue_field_position;
		u8 *pixel;
		u16 i, j;

		for (i = 0; i < mboot->framebuffer_width &&
				i < mboot->framebuffer_height; i++) {
			switch (mboot->framebuffer_bpp) {
			case 8:
				break;
			case 15:
			case 16:
				break;
			case 24:
				break;
			case 32:
				pixel = fb + mboot->framebuffer_pitch *
						i + 4 * i;
				*pixel = color;
				break;
			default:
				/* TODO Return error */
				break;
			}
		}

		for (j = 0; j < 1024; j++) {
			u32 v = j * (mboot->framebuffer_bpp / 8) + 2 *
					mboot->framebuffer_pitch;
			/* u32 i = j * 4 + 32 * 3200; */
			fb[v + 0] = 0 & 255;
			fb[v + 1] = 255 & 255;
			fb[v + 2] = 255 & 255;
		}

		for (;;)
			asm volatile ("hlt");
	} else {
		/* TODO Return error */
	}
#endif

	/* TODO Other format (UTC) */
	kprint_init();
	kprintf("\033[1;34mWelcome to Elarix %d.%d! "
			"(compiled on %s %s)\033[0;37m\n",
			RELEASE_MAJOR, RELEASE_MINOR, __DATE__, __TIME__);

	rtc_gettm(&tm);
	kprintf("Starting up at %04d-%02d-%02d %02d:%02d:%02d UTC\n", tm.year,
			tm.mon, tm.mday, tm.hour, tm.min, tm.sec);

#if 0
	usrmode_enter();

	/* TODO Start init */

	panic("attempted to kill init", 0, 0);
#endif

	/* Temporary and crappy code */
#if 1

	char cmd[64];
	u8 p = 0;

	fs_cwdir(cmd);
	kprintf("SV Shell:\n%s $ ", cmd);

	cmd[0] = '\0';

	for (;;) {
		char c;
		c = getch();

		if (c == '\b') {
			if (p < 1)
				continue;

			kprintf("%c", c);

			cmd[strlen(cmd) - 1] = '\0';
			p--;

			continue;
		} else if (c != '\n') {
			char cur[2];

			cur[0] = c;
			cur[1] = '\0';
			strcat(cmd, cur);

			kprintf("%c", c);

			if (c == '\t')
				p += 8;
			else
				p++;

			continue;
		}

		kprintf("%c", c);

		/* File system */
		if (strncmp(cmd, "ls", 2) == 0) {
			struct file *fp;
			char nbuf[NAME_MAX + 1];
			int res;

			if (strcmp(cmd, "ls") == 0)
				res = file_open(".", F_RO | F_DIR, &fp);
			else
				res = file_open(cmd + 3, F_RO | F_DIR, &fp);

			if (res == 0) {
				while (file_readdir(fp, nbuf) == 0)
					kprintf("%s ", nbuf);
				kprintf("\n");

				file_close(fp);
			}
		} else if (strcmp(cmd, "uptime") == 0) {
			u64 crap;
			struct file *fp;
			file_open("/sys/dev/tmr0", F_RO, &fp);
			file_ioctl(fp, IO_UPTIME, &crap);
			kprintf("%d\n", crap);
		} else if (strncmp(cmd, "cat", 3) == 0) {
			struct file *fp;
			char buf[4096];
			int res;

			if (strcmp(cmd, "cat") == 0)
				res = file_open(".", F_RO, &fp);
			else
				res = file_open(cmd + 4, F_RO, &fp);

			if (res == 0) {
				res = file_read(fp, buf, 4096);
				if (res > 0)
					kprintf("%s", buf);
				else
					kprintf("%d\n", res);

				file_close(fp);
			}
		} else if (strncmp(cmd, "cd", 2) == 0) {
			fs_chdir(cmd + 3);
		} else if (strcmp(cmd, "cwd") == 0) {
			fs_cwdir(cmd);
			kprintf("%s\n", cmd);
		/* } else if (strncmp(cmd, "mount", 5) == 0) {
			char dev[64];
			size_t l;

			l = strchr(cmd + 6, ' ') - cmd - 6;
			strncpy(dev, cmd + 6, l);
			dev[l] = '\0';

			sys_mount(dev, strrchr(cmd, ' ') + 1,
					"iso9660"); */
		} else if (strncmp(cmd, "unmount", 7) == 0) {
			fs_unmount(cmd + 8);
		} else if (strncmp(cmd, "mkdir", 5) == 0) {
			kprintf("%d\n", fs_mkdir(cmd + 6, 0));
		/* } else if (strcmp(cmd, "popen") == 0) {
			struct file *fp = fs_open("/sys/dev/con1", 0, 0);

			fs_write(fp, "hi\n", 0, 3);

			fs_close(fp); */

		/* Audio */
#ifdef CONFIG_AC97
		} else if (strcmp(cmd, "pac") == 0) {
			ac97_play();
#endif
#ifdef CONFIG_SB16
		} else if (strcmp(cmd, "psb") == 0) {
			sb16_play();
#endif

		/* Other */
		} else if (strcmp(cmd, "reboot") == 0) {
			reboot();
		} else if (strcmp(cmd, "halt") == 0) {
			panic(NULL, 0, 0);
		} else if (strcmp(cmd, "clear") == 0) {
			kprintf("\033[2J");
		} else if (cmd[0] != '\0') {
			kprintf("shell: command not found: %s\n", cmd);
		}

		fs_cwdir(cmd);
		kprintf("%s $ ", cmd);

		cmd[0] = '\0';
		p = 0;
	}
#endif
}
