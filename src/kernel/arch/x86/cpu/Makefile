#
# src/kernel/arch/x86/cpu/Makefile
#

kernel-o += src/kernel/arch/x86/cpu/cpu.o \
	    src/kernel/arch/x86/cpu/early_kprint.o \
	    src/kernel/arch/x86/cpu/idt.o \
	    src/kernel/arch/x86/cpu/pic.o \
	    src/kernel/arch/x86/cpu/reboot.o \
	    src/kernel/arch/x86/cpu/task.o

ifeq ($(ARCH),i386)
kernel-o += src/kernel/arch/x86/cpu/int-32.o \
	    src/kernel/arch/x86/cpu/task-32.o
endif

ifeq ($(ARCH),x86_64)
kernel-o += src/kernel/arch/x86/cpu/int-64.o \
	    src/kernel/arch/x86/cpu/task-64.o
endif
