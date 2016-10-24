#
# Makefile
#

ARCH			:= x86_64
ARCH32			= $(ARCH)
ARCHQEMU		= $(ARCH)

ifeq ($(ARCH),x86_64)
ARCHT			= x86
ARCH32			= i686
endif

ifeq ($(ARCH),i686)
ARCHT			= x86
ARCHQEMU		= i386
endif

ARCHFLAG		= -D ARCH_$(ARCH)

CC			:= $(ARCH)-elf-clemence-gcc
CC32			:= $(ARCH32)-elf-clemence-gcc
AS			:= $(ARCH)-elf-clemence-as
LD			:= $(ARCH)-elf-clemence-ld
#CC			:= $(ARCH)-elf-elara-gcc
#CC32			:= $(ARCH32)-elf-elara-gcc
#AS			:= $(ARCH)-elf-elara-as
#LD			:= $(ARCH)-elf-elara-ld
BOCHS			:= bochs
QEMU			:= qemu-system-$(ARCHQEMU)

# TODO Relocate in arch directory
# TEMP
TYPES			= src/kernel/include/sys/types.h
# TEMP
ASFLAGS			:= $(ARCHFLAG)
CFLAGS			:= $(ARCHFLAG) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Os
CFLAGS32		:= $(ARCHFLAG) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Os
LDFLAGS			:= -nostdlib -z max-page-size=4096 #-s #-Os
BOCHSFLAGS		:= -f cfg/bochs.rc -q
QEMUFLAGS		:= -m 16M --serial vc -soundhw pcspk,ac97 #-vga std #-curses #-cpu qemu32 //To test no long mode message
QEMUDBGFLAGS		:= -s -d cpu_reset,int#,cpu,exec,in_asm
KVMFLAGS		:= -enable-kvm
WGETFLAGS		:= -q --show-progress

PHONY += all
# TODO Remove qemu in final versions
all: elara iso qemu

include src/kernel/Makefile
#include src/utils/Makefile
include toolchain/Makefile

PHONY += elara
elara: kernel

PHONY += clean
# TODO Clean toolchain
clean:
	@echo -e "\033[1m> Removing binaries...\033[0m"
	@find src -type f -name '*.o' -exec rm {} \;
	@if [ -n "$(shell ls bin)" ]; then \
		 echo -e "\033[1m> Removing binaries from bin/...\033[0m"; \
		 rm bin/* > /dev/null; \
	 fi;
	@if [ -n "$(shell ls dbg)" ]; then \
		 echo -e "\033[1m> Removing debugging information from dbg/...\033[0m"; \
		 rm dbg/* > /dev/null; \
	 fi;
	@if [ -a root/grub.img ]; then \
		 echo -e "\033[1m> Removing GRUB image from root/...\033[0m"; \
		 rm root/grub.img > /dev/null; \
	 fi;
	@if [ -a root/boot/kernel ]; then \
		 echo -e "\033[1m> Removing the kernel binary from root/...\033[0m"; \
		 rm root/boot/kernel > /dev/null; \
	 fi;
	@if [[ `find root/ -type d -empty -print` ]]; then \
		 echo -e "\033[1m> Removing empty directories from root/...\033[0m"; \
		 find root/ -type d -empty -delete > /dev/null; \
	 fi;

PHONY += help
# TODO Update
help:
	@echo -e "\033[1mMakefile for Elara\033[0m"
	@echo
	@echo "Targets:"
	@echo " - all       Compile the Cito kernel and all userspace applications"
ifeq ($(ARCHT),x86)
	@echo " - bochs     Boot the Live CD iso in Bochs and launch the debugger"
endif
	@echo " - elara    Compile the Cito kernel and required userspace applications"
	@echo " - kernel      Compile the Cito kernel"
	@echo " - clean     Remove all binaries and elara.iso"
	@#echo " - fs        Convert the root directory into a FAT32 img image"
	@echo " - help      Show this help text"
	@echo " - iso       Convert the root directory into a LiveCD iso image"
	@echo " - kvm       Boot the Live CD iso in QEMU with KVM support"
	@echo " - kvmd      Boot the Live CD iso in QEMU with KVM support and debugging flags"
	@echo " - qemu      Boot the Live CD iso in QEMU"
	@echo " - qemud     Boot the Live CD iso in QEMU with debugging flags"

# TODO Implement this differently
fstree: root/tmp/
root/tmp/:
	@echo -e "\033[1m> Creating Elara root filesystem tree...\033[0m"
	@mkdir -p root/
	@mkdir -p root/app/
	@mkdir -p root/app/bin/
	@mkdir -p root/app/cfg/
	@mkdir -p root/app/inc/
	@mkdir -p root/app/lib/
	@mkdir -p root/app/shr/
	@mkdir -p root/app/tmp/
	@#mkdir -p root/boot/
	@#mkdir -p root/boot/grub/
	@mkdir -p root/cfg/
	@mkdir -p root/sys/
	@mkdir -p root/sys/dev/
	@mkdir -p root/tmp/
	@mkdir -p root/usr/
	@mkdir -p root/usr/root/
	@mkdir -p root/usr/root/.app/
	@mkdir -p root/usr/root/.app/bin/
	@mkdir -p root/usr/root/.app/cfg/
	@mkdir -p root/usr/root/.app/inc/
	@mkdir -p root/usr/root/.app/lib/
	@mkdir -p root/usr/root/.app/shr/
	@mkdir -p root/usr/root/.app/tmp/
	@mkdir -p root/usr/root/Documents/
	@mkdir -p root/usr/root/Downloads/
	@mkdir -p root/usr/root/Music/
	@mkdir -p root/usr/root/Pictures/
	@mkdir -p root/usr/root/Videos/

PHONY += iso
iso: fstree bin/elara.iso
bin/elara.iso: bin/kernel
	@echo -e "\033[1m> Copying kernel to system root...\033[0m"
	@cp bin/kernel root/boot/.
	@echo -e "\033[1m> Creating GRUB image...\033[0m"
	@grub-mkimage -p root/boot/grub -c root/boot/grub/grub.cfg -o bin/grub.img -O i386-pc biosdisk iso9660 normal multiboot ext2 boot
	@cat /usr/lib/grub/i386-pc/cdboot.img bin/grub.img > root/grub.img
	@echo -e "\033[1m> Creating Elara iso...\033[0m"
	@genisoimage -A "Elara" -input-charset "iso8859-1" -R -b grub.img -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/elara.iso root

ifeq ($(ARCHT),x86)
PHONY += bochs
bochs: iso
	@echo -e "\033[1m> Starting Bochs...\033[0m"
	@$(BOCHS) $(BOCHSFLAGS)
endif

# The 2>/dev/null is to prevent GTK warnings

PHONY += qemu
qemu: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) -cdrom bin/elara.iso 2>/dev/null

PHONY += qemud
qemud: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) -cdrom bin/elara.iso 2>/dev/null

PHONY += kvm
kvm: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(KVMFLAGS) -cdrom bin/elara.iso 2>/dev/null

PHONY += kvmd
kvmd: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) $(KVMFLAGS) -cdrom bin/elara.iso 2>/dev/null

.PHONY: $(PHONY)
