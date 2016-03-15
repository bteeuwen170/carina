/*
 *
 * Carina
 * Header for ACPI
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

#ifndef __ACPI_H_
#define __ACPI_H_

#include <stdint.h>

struct rsdp {
	char				signature[8];
	uint8_t				checksum;
	char				oemid;
	uint8_t				revision;
	uint32_t			rsdt_addr;
	uint32_t			length;
	uint64_t			xsdt_addr;
	uint8_t				checksum_ext;
	uint8_t				reserved[3];
} __attribute__ ((packed));



#endif
