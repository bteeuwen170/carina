#
# src/kernel/dev/cpu/x86/boot/Makefile
#

kernel-o += src/kernel/dev/cpu/x86/boot/entry.o

ifeq ($(ARCH),i386)
kernel-o += src/kernel/dev/cpu/x86/boot/boot-32.o
endif

ifeq ($(ARCH),x86_64)
kernel-o += src/kernel/dev/cpu/x86/boot/boot-64.o
endif
