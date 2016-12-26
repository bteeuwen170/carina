#
# src/kernel/dev/cpu/x86/boot/Makefile
#

kernel-o += src/kernel/dev/cpu/x86/boot/entry.o

ifeq ($(ARCH),i686)
kernel-o += src/kernel/dev/cpu/x86/boot/boot-32.o
endif

ifeq ($(ARCH),x86_64)
kernel-o += src/kernel/dev/cpu/x86/boot/boot-64.o
endif

#kernel-o32 += src/kernel/dev/cpu/x86/boot/paging.o
