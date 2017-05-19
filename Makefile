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


HOSTCC		:= gcc
HOSTLD		:= ld
export HOSTCC HOSTLD

CC		:= $(ARCH)-elf-elarix-gcc
CC32		:= $(ARCH32)-elf-elarix-gcc
AS		:= $(ARCH)-elf-elarix-as
LD		:= $(ARCH)-elf-elarix-gcc

BOCHS		:= bochs
QEMU		:= qemu-system-$(ARCH)


ARCHFLAGS	= -D ARCH_$(ARCH)
ifneq ($(ARCH),$(ARCHT))
ARCHFLAGS	+= -D ARCH_$(ARCHT)
endif
MAKEFLAGS	:= -s --no-print-directory

#CONFIGFLAGS	= $(shell sed -e '/\#.*$$/d;/^$$/d;s/^/-D /g;s/$$/=o/g' .config)
CONFIGFLAGS	= $(shell sed -e '/\#.*$$/d;/^$$/d;s/^/-D /g;s/$$/=o/g' .config)
$(shell sed -e '/\#.*$$/d;/^$$/d;s/$$/=o/g' .config > .config.tmp)
include .config.tmp

# TEMP TODO Relocate in arch directory
TYPES		= src/kernel/include/sys/types.h
# TEMP
ASFLAGS		:= $(RELEASEFLAGS) $(ARCHFLAGS) $(CONFIGFLAGS)
CFLAGS		:= $(RELEASEFLAGS) $(ARCHFLAGS) $(CONFIGFLAGS) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-fomit-frame-pointer
ifeq ($(ARCH),x86_64)
CFLAGS		+= -mcmodel=kernel
endif
CFLAGS32	:= $(RELEASEFLAGS) $(ARCHFLAGS) $(CONFIGFLAGS) -Wall -Wextra -Wcast-align -Werror=implicit-function-declaration -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g0 #-fno-omit-frame-pointer
LDFLAGS		:= -nostdlib -z max-page-size=4096# -s
export ASFLAGS CFLAGS CFLAGS32 LDFLAGS

HOSTCFLAGS	:= -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -g #-Os
export HOSTCFLAGS

BOCHSFLAGS	:= -f cfg/bochs.rc -q
KVMFLAGS	:= -enable-kvm
QEMUFLAGS	:= -m 64M --serial vc -soundhw ac97#,es1370 #-vga none #-curses #-cpu qemu32 # To test no long mode message
QEMUDBGFLAGS	:= -s #-d cpu_reset#,int,cpu,exec,in_asm -no-reboot
WGETFLAGS	:= -q --show-progress

include src/kernel/Makefile
#include util/Makefile
include toolchain/Makefile

PHONY += all release debug
all: release
release: CFLAGS+=-Os -s
release: LDFLAGS+=-Os -s
release: kernel iso
debug: CFLAGS+=-g
debug: kernel iso qemud

CLEAN_FILES	+= root/grub.img \
		   root/boot/kernel \
		   util/menuconfig/menuconfig \
		   .config.tmp

PHONY += clean
# TODO Clean toolchain TODO Not POSIX complient apparently
clean:
	find bin/ -type f -not -path '*/\.*' -delete -exec echo "  RM      {}" \;
	find dbg/ -type f -not -path '*/\.*' -delete -exec echo "  RM      {}" \;
	find root/ -type d -empty -delete -exec echo "  RMDIR   {}" \;
	find src/ -type f -name '*.o' -delete -exec echo "  RM      {}" \;
	find util/menuconfig -type f -name '*.o' -delete -exec echo "  RM      {}" \;
#TODO Can be done way nicer
#find ./ -type f -path '$(shell echo "${CLEAN_FILES}" | sed -e "s/ /' -o -path '/g")' -exec echo "  RM      {}" \;

PHONY += menuconfig
menuconfig:
	$(MAKE) -C util/menuconfig/ menuconfig

# TODO Implement this differently
PHONY += fstree
fstree: root/tmp/
root/tmp/:
	echo -e "\033[1m> Creating Elarix root file system tree...\033[0m"
	mkdir -p root/
	mkdir -p root/app/
	mkdir -p root/app/bin/
	mkdir -p root/app/cfg/
	mkdir -p root/app/inc/
	mkdir -p root/app/lib/
	mkdir -p root/app/shr/
	mkdir -p root/app/tmp/
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
	grub-mkimage -p root/boot/grub -c root/boot/grub/grub.cfg -o bin/grub.img -O i386-pc biosdisk boot ext2 iso9660 multiboot normal
	cat /usr/lib/grub/i386-pc/cdboot.img bin/grub.img > root/grub.img
	echo -e "\033[1m> Creating Elarix iso...\033[0m"
	#genisoimage -A "Elarix" -input-charset "utf-8" -R -b grub.img -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/elarix.iso root/
	genisoimage -A "Elarix" -input-charset "utf-8" -l -allow-leading-dots -allow-multidot -no-iso-translate -relaxed-filenames -allow-lowercase -b grub.img -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/elarix.iso root/

ifeq ($(ARCHT),x86)
PHONY += bochs
bochs: iso
	echo -e "\033[1m> Starting Bochs...\033[0m"
	$(BOCHS) $(BOCHSFLAGS) &> /dev/null
endif

# The 2>/dev/null is to suppress GTK warnings

PHONY += qemu
qemu: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) -boot d -drive file=bin/elarix.iso,if=ide,index=0,media=cdrom 2>/dev/null

PHONY += qemud
qemud: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) -boot d -drive file=bin/elarix.iso,if=ide,index=0,media=cdrom

PHONY += kvm
kvm: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(KVMFLAGS) -boot d -drive file=bin/elarix.iso,if=ide,index=0,media=cdrom 2>/dev/null

PHONY += kvmd
kvmd: iso
	echo -e "\033[1m> Starting QEMU...\033[0m"
	$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) $(KVMFLAGS) -boot d -drive file=bin/elarix.iso,if=ide,index=0,media=cdrom 2>/dev/null

.PHONY: $(PHONY)
