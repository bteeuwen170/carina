/*
 *
 * Elara
 * src/kernel/dev/acpi/acpi.h
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

#ifndef _ACPI_H
#define _ACPI_H

const char *acpi_rsdp_signature = "RSD PTR ";

struct acpi_rsdp {
	char	signature[8];
	u8	checksum;
	char	oem_id;
	u8	revision;
	u32	rsdt_addr;
	u32	length;
	u64	xsdt_addr;
	u8	checksum_ext;
	u8	reserved[3];
} __attribute__ ((packed));

struct acpi_sdt {
	char	signature[4];
	u32	length;
	u8	revision;
	u8	checksum;
	char	oem_id[6];
	char	oem_tableid[8];
	u32	oem_revision;
	u32	creator_id;
	u32	creator_revision;
} __attribute__ ((packed));

/* ACPI address spaces */
#define ACPI_GAS_AS_SM	0	/* System memory */
#define ACPI_GAS_AS_IO	1	/* System I/O */
#define ACPI_GAS_AS_PCI	2	/* PCI configuration space */
#define ACPI_GAS_AS_CTR	3	/* Embedded controller */
#define ACPI_GAS_AS_SMB	4	/* SMBus */

/* ACPI access sizes */
#define ACPI_GAS_ASIZE	0	/* Undefined */
#define ACPI_GAS_ASIZEB	1	/* Byte (1 byte) */
#define ACPI_GAS_ASIZEW	2	/* Word (2 bytes) */
#define ACPI_GAS_ASIZED	3	/* Dword (4 bytes) */
#define ACPI_GAS_ASIZEQ	4	/* Qword (8 bytes) */

struct acpi_gas {
	u8	addr_space;
	u8	bit_width;
	u8	bit_off;
	u8	asize;
	u64	addr;
} __attribute__ ((packed));

/* FACP - ACPI V3b 5.2.9 */
struct acpi_fadt {
	struct acpi_sdt	header;

	u32		firmware_ctrl;
	u32		dsdt;

	u8		reserved0;

	u8		pm_profile;

	u16		sci_int;
	u32		smi_cmd;

	u8		acpi_enable;
	u8		acpi_disable;

	u8		s4bios_req;
	u8		pstate_ctrl;

	u32		pm1a_evt_blk;
	u32		pm1b_evt_blk;
	u32		pm1a_ctrl_blk;
	u32		pm1b_ctrl_blk;
	u32		pm2_ctrl_blk;
	u32		pm_timer_blk;
	u32		gpe0_blk;
	u32		gpe1_blk;

	u8		pm1_evt_len;
	u8		pm1_ctrl_len;
	u8		pm2_ctrl_len;
	u8		pm_timer_len;
	u8		gpe0_blk_len;
	u8		gpe1_blk_len;
	u8		gpe1_base;
	u8		cst_cnt;

	u16		p2_lat;
	u16		p3_lat;

	u16		flush_size;
	u16		flush_stride;

	u8		duty_off;
	u8		duty_width;

	u8		rtc_aday;
	u8		rtc_amon;
	u8		rtc_cent;

	u16		boot_flags;

	u8		reserved1;

	u32		flags;	/* TODO table 5-10 ACPIv3b */

	struct acpi_gas	reset_reg;
	u8		reset_val;

	u8		reserved2[3];

	struct acpi_gas	pm1a_evt_blk_ext;
	struct acpi_gas	pm1b_evt_blk_ext;
	struct acpi_gas	pm1a_ctrl_blk_ext;
	struct acpi_gas	pm1b_ctrl_blk_ext;
	struct acpi_gas	pm2_ctrl_blk_ext;
	struct acpi_gas	pm_timer_blk_ext;
	struct acpi_gas	gpe0_blk_ext;
	struct acpi_gas	gpe1_blk_ext;
} __attribute__ ((packed));

/* TODO Table 5-19 */
#define APIC_T_LAPIC	0
#define APIC_T_IOAPIC	1
#define APIC_T_ISO	2
#define APIC_T_NMI	3
#define APIC_T_LANMI	4
#define APIC_T_LAADDR	5
#define APIC_T_IOSAPIC	6
#define APIC_T_LSPAIC	7
#define APIC_T_PIS	8

/* APIC - ACPI V3b 5.2.11.4 */
struct acpi_madt {
	struct acpi_sdt	header;

	u32		lapic_addr;
	u32		flags; /* TODO table 5-18 ACPIv3b */
} __attribute__ ((packed));

struct apic_head {
	u8	type;
	u8	length;
} __attribute__ ((packed));

/* LAPIC - ACPI V3b 5.2.11.5 */
struct apic_lapic {
	struct apic_head	header;

	u8			acpi_cpuid;
	u8			id;
	u32			flags; /* TODO table 5-21 ACPIv3b */
} __attribute__ ((packed));

/* I/O APIC - ACPI V3b 5.2.11.6 */
struct apic_ioapic {
	struct apic_head	header;

	u8			id;
	u8			reserved0;
	u32			addr;
	u32			gsib;
} __attribute__ ((packed));

/* ISO - ACPI V3b 5.2.11.8 */
struct apic_iso {
	struct apic_head	header;

	u8			bus;
	u8			src;
	u32			gsib;
	u16			flags; /* TODO table 5-24 ACPIv2b */
} __attribute__ ((packed));

void acpi_init(void);

#endif
