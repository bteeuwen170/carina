/*
 *
 * Carina
 * Main Cito Kernel
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

#include <rtc/cmos.h>
#include <kernel/mboot.h>
#include <kernel/issue.h>
#include <cpu.h>
#include <char/serial/serial.h>
#include <kernel/reboot.h>
#include <timer/pit.h>
#include <sound/pcspk.h>
#include <video/fb.h>
#include <kbd/kbd.h>
#include <kernel/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <8259.h>
#include <video/vga.h>
#include <pci/pci.h>
#include <kernel/print.h>
#include <video/vesa.h>
#include <sound/ac97.h>

extern void usrmode_enter();

void kernel_main(struct mboot_info *mboot)
{
	/* Initialize early video and debugging hardware */
	vga_init();
	serial_init(COM0);

	/* TODO Other format (UTC) */
	kprintf(KP_INFO, 0, "Welcome to Carina! (compiled on %s %s)\n",
			__DATE__, __TIME__);

	kprintf(KP_INFO, 0, "Command line: %s\n", mboot->cmdline);

	/* Initialize mandatory hardware */
	pic_remap();
	//pic_disable();
	idt_init();
	//tss_init();
	//lapic_init();
	//ioapic_init();

	mm_init();

	/* TODO Move */
	kprintf(KP_INFO, "mem", "%u KB base memory\n", mboot->mem_lower);
	kprintf(KP_INFO, "mem", "%u MB extended memory\n", mboot->mem_upper / 1024);

#if 0
	/* Get memory map from Multiboot structure */
	kprintf(KP_INFO, 0, "Carina has been loaded by %s\n", mboot->boot_loader_name);
	struct mboot_mmap *mmap = (struct mboot_mmap *) mboot->mmap_addr;
	while ((u64) mmap < mboot->mmap_addr + mboot->mmap_length) {
		mmap = (struct mboot_mmap *) ((u64) mmap + mmap->size + sizeof(u32));
		kprintf(KP_INFO, "mem", "size = 0x%x, addr = 0x%x%x, len = 0x%x%x, type = 0x%x\n",
				(u32) mmap->size, mmap->addr >> 32, mmap->addr & 0xFFFFFFFF, mmap->len >> 32, mmap->len & 0xFFFFFFFF, (u32) mmap->type);
	}

	for (mmap = (struct mboot_mmap *) mboot->mmap_addr; (u64) mmap < mboot->mmap_addr + mboot->mmap_length; mmap = (struct mboot_mmap *) ((u64) mmap + mmap->size + sizeof(mmap->size))) {
		kprintf(KP_INFO, "mem", "size = 0x%x, addr = 0x%x%x, len = 0x%x%x, type = 0x%x\n",
				(u32) mmap->size, mmap->addr >> 32, mmap->addr & 0xFFFFFFFF, mmap->len >> 32, mmap->len & 0xFFFFFFFF, (u32) mmap->type);
	}
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
		u32 color = ((1 << mboot->framebuffer_blue_mask_size) - 1) << mboot->framebuffer_blue_field_position;
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
				pixel = fb + mboot->framebuffer_pitch * i + 4 * i;
				*pixel = color;
				break;
			default:
				/* TODO Return error */
				break;
			}
		}

		for (j = 0; j < 1024; j++) {
			u32 v = j * (mboot->framebuffer_bpp / 8) + 2 * mboot->framebuffer_pitch;
			//u32 i = j * 4 + 32 * 3200;
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

	//cpu_info(); /* FIXME Secure mboot structure first */

	/* Register PCI handlers */
	ac97_reghandler();

	pci_scan();

	/* Initialize remaining hardware */
	//acpi_init();
	rtc_init();
	pit_init();
	kbd_enable();

	asm volatile ("sti");
	kprintf(KP_DBG, "x86", "Interrupts enabled\n");

#if 0
	usrmode_enter();

	kprintf(KP_DBG, "x86", "In usermode\n");

	for (;;)
		asm volatile ("hlt");

	/* TODO Start init */

	panic("Init was killed", 0);
#else
	/* Temporary and crappy code */
	char cmd[64];
	u8 i, p = 0;

	cmd[0] = '\0';

	prints("SV Shell:\n$ ");

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

		if (strcmp(cmd, "beep") == 0) {
			pcspk_play(835);
			sleep(10);
			pcspk_stop();
		/* CRAP */
		} else if (strcmp(cmd, "fj") == 0) {
			pcspk_fj();
		} else if (strcmp(cmd, "mi") == 0) {
			pcspk_mi();
		} else if (strcmp(cmd, "hc") == 0) {
			pcspk_hc();
		} else if (strcmp(cmd, "acri") == 0) {
			pcspk_acri();
		/* ENDCRAP */
		} else if (strcmp(cmd, "ac97 play") == 0) {
			ac97_play();
			kprintf(KP_DBG,"ac97", "WAV playing\n");
		} else if (strcmp(cmd, "reboot") == 0) {
			reboot();
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
