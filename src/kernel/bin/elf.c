/*
 *
 * Elarix
 * src/kernel/bin/elf.c
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

#include <elf.h>
#include <kernel.h>

#include <asm/cpu.h>

#ifdef CONFIG_EXEC_VERBOSE
static const char devname[] = "elf";
#endif

int elf_read_header(struct elf_header *ehp)
{
#ifdef CONFIG_EXEC_VERBOSE
	dprintf("loading ELF file\n");
#endif

	if (ehp->magic[0] != 0x7F ||
			ehp->magic[1] != 'E' ||
			ehp->magic[2] != 'L' ||
			ehp->magic[3] != 'F') {
#ifdef CONFIG_EXEC_VERBOSE
	dprintf(KP_CON "invalid magic number: %#x%c%c%c\n", ehp->magic[0],
			ehp->magic[1], ehp->magic[2], ehp->magic[3]);
#endif
		return 0;
	}

#ifdef CONFIG_EXEC_VERBOSE
	dprintf(KP_CON "class: %u\n", ehp->class);
#endif

	if (ehp->data != 1) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid endianness: %u\n", ehp->data);
#endif
		return 0;
	}

	if (ehp->version0 != 1 || ehp->version1 != 1) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid version number(s): %u, %u\n",
				ehp->version0, ehp->version1);
#endif
		return 0;
	}

#ifdef CONFIG_EXEC_VERBOSE
	dprintf(KP_CON "target ABI: %#x\n", ehp->abi);
#endif

	if (ehp->machine !=
#ifdef ARCH_i386
	0x03
#elif defined(ARCH_x86_64)
	0x3E
#endif
	) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid architecture: %#x\n", ehp->machine);
#endif
		return 0;
	}

	return ehp->class;
}

int elf32_load(struct elf_header32 *ehp)
{
	if (ehp->header_size != sizeof(struct elf_header32)) {
		return 0;
	}

	if (ehp->program_size != sizeof(struct elf_program32)) {
		return 0;
	}

	if (ehp->program_len * sizeof(struct elf_program32) > PAGE_SIZE) {
		return 0;
	}

	return 1;
}

int elf64_load(struct elf_header64 *ehp)
{
	struct elf_program64 *epp;
	u16 i;

	if (ehp->header_size != sizeof(struct elf_header64)) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid file header size: %u\n",
				ehp->header_size);
#endif
		return 0;
	}

	if (ehp->program_size != sizeof(struct elf_program64)) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid program header size: %u\n",
				ehp->program_size);
#endif
		return 0;
	}

	if (ehp->program_len * sizeof(struct elf_program64) > PAGE_SIZE) {
#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "invalid program header length: %u\n",
				ehp->program_len);
#endif
		return 0;
	}

	for (i = 0; i < ehp->program_len; i++) {
		epp = (struct elf_program64 *) ((char *) ehp +
				ehp->program_off + i * ehp->program_size);

		if (epp->type != ELF_P_TYPE_LOAD)
			continue;

#ifdef CONFIG_EXEC_VERBOSE
		dprintf(KP_CON "vaddr: %#lx, paddr: %#lx, size: %u\n",
				epp->vaddr, epp->paddr, epp->size);
#endif
		return 1;
	}

	dprintf(KP_CON "unable to find LOAD program header\n");

	return 0;
}
