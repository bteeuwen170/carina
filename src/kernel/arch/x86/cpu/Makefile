#
# src/kernel/arch/x86/cpu/Makefile
#

kernel-o += src/kernel/arch/x86/cpu/8259.o \
	    src/kernel/arch/x86/cpu/cpu.o \
	    src/kernel/arch/x86/cpu/idt.o \
	    src/kernel/arch/x86/cpu/reboot.o

kernel-o32 += src/kernel/arch/x86/cpu/gdt32.o \
	      src/kernel/arch/x86/cpu/paging32.o \
	      src/kernel/arch/x86/cpu/tss32.o

ifeq ($(ARCH),i386)
kernel-o += src/kernel/arch/x86/cpu/int-32.o
kernel-o += src/kernel/arch/x86/cpu/task-32.o
endif

ifeq ($(ARCH),x86_64)
kernel-o += src/kernel/arch/x86/cpu/int-64.o
kernel-o += src/kernel/arch/x86/cpu/task-64.o
endif
