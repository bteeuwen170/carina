#
# Makefile
#

# TODO Improve cross compiling mechanics

RELEASE_MAJOR	= 0
RELEASE_MINOR	= 1
RELEASEFLAGS	= -D RELEASE_MAJOR=$(RELEASE_MAJOR) -D RELEASE_MINOR=$(RELEASE_MINOR)

ARCH		:= x86_64
ARCH32		= $(ARCH)

ifeq ($(ARCH),i386)
ARCHT		= x86
endif

ifeq ($(ARCH),x86_64)
ARCHT		= x86
ARCH32		= i386
endif


# Binaries

HOSTCC		:= gcc
HOSTLD		:= ld
export HOSTCC HOSTLD

CC		:= $(ARCH)-elf-elarix-gcc
CC32		:= $(ARCH32)-elf-elarix-gcc
AS		:= $(ARCH)-elf-elarix-as
LD		:= $(ARCH)-elf-elarix-ld

BOCHS		:= bochs
QEMU		:= qemu-system-$(ARCH)


# Flags

ARCHFLAG	= -D ARCH_$(ARCH)
MAKEFLAGS	= -s --no-print-directory

CONFIGFLAGS	= $(shell sed -e '/^\s*\\\#/d' -e '/^\s*$$/d' -e 's/^/-D /g' .config)
#MAKEFLAGS	+= $(shell sed -e '/^\s*\\\#/d' -e '/^\s*$$/d' -e 's/^/\n/g' .config)
include .config

# TEMP TODO Relocate in arch directory
TYPES		= src/kernel/include/sys/types.h
# TEMP
ASFLAGS		:= $(RELEASEFLAGS) $(ARCHFLAG) $(CONFIGFLAGS)
CFLAGS		:= $(RELEASEFLAGS) $(ARCHFLAG) $(CONFIGFLAGS) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Wno-unused-parameter -Wno-return-type #-Os
CFLAGS32	:= $(RELEASEFLAGS) $(ARCHFLAG) $(CONFIGFLAGS) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Os
LDFLAGS		:= -nostdlib -z max-page-size=4096 #-s #-Os

BOCHSFLAGS	:= -f cfg/bochs.rc -q
KVMFLAGS	:= -enable-kvm
QEMUFLAGS	:= -m 8M --serial vc -soundhw pcspk,ac97,sb16 #-vga std #-curses #-cpu qemu32 # To test no long mode message
QEMUDBGFLAGS	:= -s -d cpu_reset,int#,cpu,exec,in_asm
WGETFLAGS	:= -q --show-progress

include src/kernel/Makefile
#include util/Makefile
include toolchain/Makefile

PHONY += all debug
all: kernel iso
debug: kernel iso qemu

CLEAN_FILES	+= root/grub.img \
		   root/boot/kernel \
		   util/menu/menu

PHONY += clean
# TODO Clean toolchain TODO Not POSIX complient apperently
clean:
	find bin/ -type f -not -path '*/\.*' -delete -exec echo "  RM      {}" \;
	find dbg/ -type f -not -path '*/\.*' -delete -exec echo "  RM      {}" \;
	find root/ -type d -empty -delete -exec echo "  RMDIR   {}" \;
	find src/ -type f -name '*.o' -delete -exec echo "  RM      {}" \;
#TODO Can be done way nicer
#find ./ -type f -path '$(shell echo "${CLEAN_FILES}" | sed -e "s/ /' -o -path '/g")' -exec echo "  RM      {}" \;

PHONY += menuconfig
menuconfig:
	$(MAKE) -C util/menu/ all

# TODO Implement this differently
PHONY += fstree
fstree: root/tmp/
root/tmp/:
	echo -e "\033[1m> Creating Elarix root filesystem tree...\033[0m"
	mkdir -p root/
	mkdir -p root/app/
	mkdir -p root/app/bin/
	mkdir -p root/app/cfg/
	mkdir -p root/app/inc/
	mkdir -p root/app/lib/
	mkdir -p root/app/shr/
	mkdir -p root/app/tmp/
	#mkdir -p root/boot/
	#mkdir -p root/boot/grub/
	mkdir -p root/cfg/
	mkdir -p root/sys/
	mkdir -p root/sys/dev/
	mkdir -p root/tmp/
	mkdir -p root/usr/
	mkdir -p root/usr/root/
	mkdir -p root/usr/root/.app/
	mkdir -p root/usr/root/.app/bin/
	mkdir -p root/usr/root/.app/cfg/
	mkdir -p root/usr/root/.app/inc/
	mkdir -p root/usr/root/.app/lib/
	mkdir -p root/usr/root/.app/shr/
	mkdir -p root/usr/root/.app/tmp/
	mkdir -p root/usr/root/Documents/
	mkdir -p root/usr/root/Downloads/
	mkdir -p root/usr/root/Music/
	mkdir -p root/usr/root/Pictures/
	mkdir -p root/usr/root/Videos/

PHONY += iso
iso: fstree bin/elarix.iso
bin/elarix.iso: bin/kernel
	echo -e "\033[1m> Copying kernel to system root...\033[0m"
	cp bin/kernel root/boot/.
	echo -e "\033[1m> Creating GRUB image...\033[0m"
	grub-mkimage -p root/boot/grub -c root/boot/grub/grub.cfg -o bin/grub.img -O i386-pc biosdisk iso9660 normal multiboot ext2 boot
	cat /usr/lib/grub/i386-pc/cdboot.img bin/grub.img > root/grub.img
	echo -e "\033[1m> Creating Elarix iso...\033[0m"
	genisoimage -A "Elarix" -input-charset "iso8859-1" -R -b grub.img -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/elarix.iso root

ifeq ($(ARCHT),x86)
PHONY += bochs
bochs: iso
	echo -e "\033[1m> Starting Bochs...\033[0m"
	$(BOCHS) $(BOCHSFLAGS)
endif

# The 2>/dev/null is to suppress GTK warnings

PHONY += qemu
qemu: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) -cdrom bin/elarix.iso 2>/dev/null

PHONY += qemud
qemud: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) -cdrom bin/elarix.iso 2>/dev/null

PHONY += kvm
kvm: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(KVMFLAGS) -cdrom bin/elarix.iso 2>/dev/null

PHONY += kvmd
kvmd: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) $(KVMFLAGS) -cdrom bin/elarix.iso 2>/dev/null

.PHONY: $(PHONY)
