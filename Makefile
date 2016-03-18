##
## Variable Definitions
##

ARCH				= x86_64
CRC					= $(ARCH)-elf-

AS					= $(CRC)as
BOCHS				= bochs
CC					= $(CRC)gcc
MKFS				= mkfs.cfs
QEMU				= qemu-system-$(ARCH)
LD					= $(CRC)ld

CFLAGS				= -g -Wall -Wextra -Wcast-align -fdiagnostics-color=auto -std=gnu99 -O2 -ffreestanding -nostdlib -lgcc -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
BFLAGS				= -f cfg/bochs.rc -q
DFLAGS				= -s -d cpu_reset,cpu,exec,int,in_asm
KFLAGS				= -enable-kvm
LFLAGS				= -nostdlib -nodefaultlibs -z max-page-size=0x1000
MFLAGS				= -F 32 -v
QFLAGS				= -m 16M --serial vc -soundhw pcspk -vga std

include ./src/cito/arch/$(ARCH)/Makefile
include ./src/cito/Makefile

.PHONY: all bochs carina cito clean fs help iso kvm kvmd qemu qemud

all: carina iso qemu


##
## Base System
##

carina: cito


##
## Clean
##

clean:
	@echo -e "\033[1m> Cleaning the serial output file...\033[0m"
	@truncate -s 0 ./dbg/serial
	@echo -e "\033[1m> Removing binaries...\033[0m"
	@find ./src -type f -name '*.o' -exec rm {} \;
	@echo -e "\033[1m> Removing Carina iso...\033[0m"
	@if [ -a ./bin/carina.iso ]; then \
		 rm ./bin/carina.iso; \
	 fi;
	@echo -e "\033[1m> Removing the Cito kernel binaries...\033[0m"
	@if [ -a ./bin/cito.bin ]; then \
		 rm ./bin/cito.bin; \
	 fi;
	@if [ -a ./hdd/boot/cito.bin ]; then \
		 rm ./hdd/boot/cito.bin; \
	 fi;
	@echo -e "\033[1mDone!\033[0m"


##
## Help
##

help:
	@echo -e "\033[1mMakefile for Carina\033[0m"
	@echo
	@echo "Targets:"
	@echo " - all       Compile the Cito kernel and all userspace applications"
	@echo " - bochs     Boot the Live CD iso in Bochs and launch the debugger"
	@echo " - carina    Compile the Cito kernel and required userspace applications"
	@echo " - cito      Compile the Cito kernel"
	@echo " - clean     Remove all binaries and carina.iso"
	@#echo " - fs        Convert the hdd directory into a FAT32 img image"
	@echo " - help      Show this help text"
	@echo " - iso       Convert the hdd directory into a LiveCD iso image"
	@echo " - kvm       Boot the Live CD iso in QEMU with KVM support"
	@echo " - kvmd      Boot the Live CD iso in QEMU with KVM support and debugging flags"
	@echo " - qemu      Boot the Live CD iso in QEMU"
	@echo " - qemud     Boot the Live CD iso in QEMU with debugging flags"


##
## Building
##

cito: $(OBJS_CITO) cfg/cito.ld
	@echo -e "\033[1m> Linking the Cito kernel...\033[0m"
	@$(LD) $(LFLAGS) -T ./cfg/cito.ld -o ./bin/cito.bin $(OBJS_CITO)

%.o: %.c
	@echo -e "\033[1m> Compiling \033[0;32m$<\033[1m...\033[0m"
	@$(CC) $(CFLAGS) -c $< -I $(INC_CITO) -o $@

%.o: %.s
	@echo -e "\033[1m> Assembling \033[0;32m$<\033[1m...\033[0m"
	@$(AS) $< -o $@


##
## Make ISO Image
##

#fs: hdd/boot/cito.bin
#	@echo -e "\033[1m> Creating FAT32 img...\033[0m"
#	@dd if=/dev/zero of=./bin/carina.img bs=1M count=40
#	@$(MKFS) $(MFLAGS) ./bin/carina.img

iso: ./bin/cito.bin
	@echo -e "\033[1m> Copying cito kernel to system root...\033[0m"
	@cp ./bin/cito.bin ./hdd/boot/.
	@echo -e "\033[1m> Creating Carina iso...\033[0m"
	@grub-mkrescue -o ./bin/carina.iso ./hdd


##
## QEMU
##

bochs: iso
	@echo -e "\033[1m> Starting Bochs...\033[0m"
	@$(BOCHS) $(BFLAGS)

qemu: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QFLAGS) -cdrom ./bin/carina.iso

qemud: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QFLAGS) $(DFLAGS) -cdrom ./bin/carina.iso

kvm: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QFLAGS) $(KFLAGS) -cdrom ./bin/carina.iso

kvmd: iso
	@echo -e "\033[1m> Starting QEMU...\033[0m"
	@$(QEMU) $(QFLAGS) $(DFLAGS) $(KFLAGS) -cdrom ./bin/carina.iso
