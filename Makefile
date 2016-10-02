##
## Variable Definitions
##

ARCH			:= x86_64
ARCH32			= $(ARCH)
ARCHQEMU		= $(ARCH)
ARCHFLAG		= -D ARCH_$(ARCH)

ifeq ($(ARCH),x86_64)
ARCHT			= x86
ARCH32			= i686
endif

ifeq ($(ARCH),i686)
ARCHT			= x86
ARCHQEMU		= i386
endif

CC			:= $(ARCH)-elf-gcc
CC32			:= $(ARCH32)-elf-gcc
AS			:= $(ARCH)-elf-as
LD			:= $(ARCH)-elf-ld
BOCHS			:= bochs
QEMU			:= qemu-system-$(ARCHQEMU)

# TODO Relocate in arch directory
# TEMP
TYPES			= src/kernel/include/kernel/types.h
# TEMP
ASFLAGS			:= $(ARCHFLAG)
CFLAGS			:= $(ARCHFLAG) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Os
CFLAGS32		:= $(ARCH32FLAG) -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -fno-asynchronous-unwind-tables -std=gnu89 -ffreestanding -nostdlib -lgcc -include $(TYPES) -mno-red-zone -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -mno-avx -g #-Os
LDFLAGS			:= -nostdlib -z max-page-size=4096 #-s #-Os
BOCHSFLAGS		:= -f cfg/bochs.rc -q
QEMUFLAGS		:= -m 16M --serial vc -soundhw pcspk,ac97 #-vga std #-curses #-cpu qemu32 //To test no long mode message
QEMUDBGFLAGS		:= -s -d cpu_reset,int#,cpu,exec,in_asm
KVMFLAGS		:= -enable-kvm

include src/Makefile

PHONY += all
# TODO Remove qemu in final versions
all: carina iso qemu


##
## Base System
##

PHONY += carina
carina: kernel


##
## Clean
##

PHONY += clean
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


##
## Help
##

PHONY += help
help:
	@echo -e "\033[1mMakefile for Carina\033[0m"
	@echo
	@echo "Targets:"
	@echo " - all       Compile the Cito kernel and all userspace applications"
ifeq ($(ARCHT),x86)
	@echo " - bochs     Boot the Live CD iso in Bochs and launch the debugger"
endif
	@echo " - carina    Compile the Cito kernel and required userspace applications"
	@echo " - kernel      Compile the Cito kernel"
	@echo " - clean     Remove all binaries and carina.iso"
	@#echo " - fs        Convert the root directory into a FAT32 img image"
	@echo " - help      Show this help text"
	@echo " - iso       Convert the root directory into a LiveCD iso image"
	@echo " - kvm       Boot the Live CD iso in QEMU with KVM support"
	@echo " - kvmd      Boot the Live CD iso in QEMU with KVM support and debugging flags"
	@echo " - qemu      Boot the Live CD iso in QEMU"
	@echo " - qemud     Boot the Live CD iso in QEMU with debugging flags"


##
## Building TODO Move to kernel ?
##

%32.o: %32.c
	@echo -e "\033[1;37m> Compiling \033[0;32m$<\033[1m...\033[0m"
	@$(CC32) $(CFLAGS32) -c $< $(kernel-i) -o $@
ifeq ($(ARCH),x86_64)
	@objcopy -O elf64-x86-64 $@ $@.64
	@mv $@.64 $@
endif

%.o: %.c
	@echo -e "\033[1;37m> Compiling \033[0;32m$<\033[1m...\033[0m"
	@$(CC) $(CFLAGS) -c $< $(kernel-i) -o $@

%.o: %.S
	@echo -e "\033[1;37m> Assembling \033[0;32m$<\033[1m...\033[0m"
	@$(CC) $(ASFLAGS) -c $< -o $@


##
## Kernel
##

PHONY += kernel
kernel: bin/kernel
bin/kernel: $(kernel-o) $(kernel-o32)
	@echo -e "\033[1m> Linking \033[0;32m$@\033[1m...\033[0m"
	@$(LD) $(LDFLAGS) -T src/kernel/link.ld -o bin/kernel $(kernel-o) $(kernel-o32)


##
## Make ISO Image
##

#fs: root/boot/kernel
#	@echo -e "\033[1m> Creating FAT32 img...\033[0m"
#	@dd if=/dev/zero of=bin/carina.img bs=1M count=40
#	@$(MKFS) $(MFLAGS) bin/carina.img

#FIXME Don't call this if iso is already present
PHONY += iso
iso: bin/kernel
	@echo -e "\033[1m> Copying kernel to system root...\033[0m"
	@cp bin/kernel root/boot/.
	@echo -e "\033[1m> Creating GRUB image...\033[0m"
	@grub-mkimage -p root/boot/grub -c root/boot/grub/grub.cfg -o bin/grub.img -O i386-pc biosdisk iso9660 normal multiboot ext2 boot
	@cat /usr/lib/grub/i386-pc/cdboot.img bin/grub.img > root/grub.img
	@echo -e "\033[1m> Creating Carina iso...\033[0m"
	@genisoimage -A "Carina" -input-charset "iso8859-1" -R -b grub.img -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/carina.iso root


##
## QEMU
##

ifeq ($(ARCHT),x86)
PHONY += bochs
bochs: iso
	@echo -e "\033[1m> Starting Bochs...\033[0m"
	@$(BOCHS) $(BOCHSFLAGS)
endif

PHONY += qemu
qemu: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) -cdrom bin/carina.iso

PHONY += qemud
qemud: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) -cdrom bin/carina.iso

PHONY += kvm
kvm: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(KVMFLAGS) -cdrom bin/carina.iso

PHONY += kvmd
kvmd: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QEMUFLAGS) $(QEMUDBGFLAGS) $(KVMFLAGS) -cdrom bin/carina.iso

.PHONY: $(PHONY)
