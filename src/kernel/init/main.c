/*
 *
 * Elara
 * src/kernel/init/main.c
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

#include <issue.h>
#include <lock.h>
#include <mboot.h>
#include <print.h>
#include <reboot.h>
#include <sys/time.h>

#include <asm/8259.h>
#include <asm/cpu.h>

#include <block/ide/ata.h>
#include <char/serial/serial.h>
#include <fs/ramfs.h>
#include <kbd/kbd.h>
#include <pci/pci.h>
#include <rtc/cmos.h>
#include <sound/ac97.h>
/* #include <sound/sb16.h> */
#include <sound/pcspk.h>
#include <timer/pit.h>
#include <video/fb.h>
#include <video/vesa.h>
#include <video/vga.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void usrmode_enter();

void kernel_main(struct mboot_info *mboot)
{
	spinlock_t lock = SPINLOCK;

	spin_lock(lock);

	/* struct mboot_info *mboot = kmalloc(sizeof(struct mboot_info)); */
	/* memcpy(mboot, _mboot, sizeof(struct mboot_info)); */
	/* Initialize early video and debugging hardware */
	vga_init();
	serial_init(COM0);

	/* Initialize mandatory hardware */
	pic_remap();
	/* pic_disable(); */
	idt_init();
	/* tss_init(); */
	/* lapic_init(); */
	/* ioapic_init(); */

	/* TODO Other format (UTC) */
	kprintf(KP_INFO, "cpu0", "Welcome to Elara! (compiled on %s %s)\n",
			__DATE__, __TIME__);
	/* TODO Actually get starting cpu */

	/* TODO Move */
	kprintf(KP_INFO | KP_CON, 0, "Elara has been loaded by %s\n",
			mboot->boot_loader_name);
	kprintf(KP_INFO | KP_CON, 0, "cmdline: %s\n", mboot->cmdline);

	mm_init(mboot->mmap_addr, mboot->mmap_len);

#if 0
	/* TODO Move */
	kprintf(KP_INFO, "mem", "%u KB base memory\n", mboot->mem_lo);
	kprintf(KP_INFO, "mem",
			"%u MB extended memory\n", mboot->mem_hi / 1024);
#endif

#if 0
	/* Initialize video hardware properly */
	kprintf(KP_DBG, "fb", "addr is %#x\n", mboot->framebuffer_addr);
	kprintf(KP_DBG, "fb", "bpp is %#x\n", mboot->framebuffer_bpp);
	kprintf(KP_DBG, "fb", "pitch is %#x\n", mboot->framebuffer_pitch);
	kprintf(KP_DBG, "fb", "type is %#x\n", mboot->framebuffer_type);

	/* TODO Switch */
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

	/* cpu_info(); */

	/* Initialize remaining hardware */
	/* acpi_init(); */
	rtc_init();
	pit_init();
	kbd_enable();

	/* Register PCI handlers */
	ide_reghandler();
	ac97_reghandler();

	asm volatile ("sti");
	kprintf(KP_DBG, "cpu0", "Interrupts enabled\n");

	pci_scan();

	/* sb16_init(); */

	/* Temporary and crappy code */
#if 0
	usrmode_enter();

	/* kprintf(KP_DBG, "x86", "In usermode\n"); */

	for (;;)
		asm volatile ("hlt");

	/* TODO Start init */

	panic("Init was killed", 0, 0);
#else
	char cmd[64];
	u8 i, p = 0;
	/* int ipp; */

	cmd[0] = '\0';

	prints("\e[1;34mSV Shell:\n$ ");

	for (;;) {
		char c;
		c = getch();

		if (c == '\b') {
			if (p < 1)
				continue;

			printc(c);

			cmd[strlen(cmd) - 1] = '\0';
			p--;

			continue;
		} else if (c != '\n') {
			char cur[2];

			cur[0] = c;
			cur[1] = '\0';
			strcat(cmd, cur);

			printc(c);

			p++;

			continue;
		}

		printc(c);

		/* File system */
		if (strcmp(cmd, "fs init") == 0) {
#if 0
			u32 sdf = 0;
			int res = ramfs_get(4096, &sdf, &ipp);

			kprintf(KP_DBG, "fs", "ret: %d  dev: %u\n", res, sdf);
		} else if (strcmp(cmd, "ls") == 0) {
			int n = 0;
			while (ramfs_read_dir(ipp, n) != NULL)
				kprintf(0,0, "%s\n",
						ramfs_read_dir(ipp, n++)->name);
#endif

		/* Audio */
		} else if (strcmp(cmd, "beep") == 0) {
			pcspk_play(835);
			sleep(10);
			pcspk_stop();
		} else if (strcmp(cmd, "ac97 play") == 0) {
			ac97_play();
		/* } else if (strcmp(cmd, "sb16 play") == 0) {
			sb16_play(); */
		} else if (strcmp(cmd, "fj") == 0) {
			pcspk_fj();
		} else if (strcmp(cmd, "mi") == 0) {
			pcspk_mi();
		} else if (strcmp(cmd, "hc") == 0) {
			pcspk_hc();
		} else if (strcmp(cmd, "acri") == 0) {
			pcspk_acri();

		/* Other */
		} else if (strcmp(cmd, "reboot") == 0) {
			reboot();
		} else if (strcmp(cmd, "halt") == 0) {
			panic("halt", 0, 0);
		} else if (strcmp(cmd, "clear") == 0) {
			vga_clear();
		} else if (strcmp(cmd, "uptime") == 0) {
			kprintf(0, 0, "uptime: %d seconds\n", uptime());
		} else if (cmd[0] != '\0') {
			kprintf(0, 0, "shell: command not found: %s\n", cmd);
		}

		p = 0;

		prints("$ ");

		for (i = 1; i < strlen(cmd); i++)
			cmd[i] = 0;
		cmd[0] = '\0';
	} 
#endif
}

void usermode_main(void)
{
	asm volatile ("cli");
	kprintf(KP_DBG, "x86", "User mode\n");
	for (;;)
		asm volatile ("hlt");
}
