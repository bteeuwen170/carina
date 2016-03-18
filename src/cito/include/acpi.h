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

#include <stdlib.h>

struct rsdp {
	uint8				signature[8];
	uint8				checksum;
	uint8				oemid;
	uint8				revision;
	uint32				rsdt_addr;
	uint32				length;
	uint64				xsdt_addr;
	uint8				checksum_ext;
	uint8				reserved[3];
} __attribute__ ((packed));



#endif
