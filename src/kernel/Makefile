#
# src/kernel/Makefile
#

include src/kernel/dev/Makefile
include src/kernel/fs/Makefile
include src/kernel/init/Makefile
include src/kernel/ipc/Makefile
include src/kernel/kernel/Makefile
include src/kernel/lib/Makefile
include src/kernel/mm/Makefile

kernel-i += -I src/kernel/dev
kernel-i += -I src/kernel/include
kernel-i += -I src/kernel/lib

%32.o: %32.c
	echo -e "  CC32    $<"
	$(CC32) $(CFLAGS32) -c $< $(kernel-i) -o $@
ifeq ($(ARCH),x86_64)
	objcopy -O elf64-x86-64 $@ $@.64
	mv $@.64 $@
endif

%.o: %.c
	echo -e "  CC      $<"
	$(CC) $(CFLAGS) -c $< $(kernel-i) -o $@

%.o: %.S
	echo -e "  AS      $<"
	$(CC) $(ASFLAGS) -c $< -o $@


PHONY += kernel
kernel: bin/kernel
bin/kernel: $(kernel-o) $(kernel-o32)
	echo -e "  LD      $<"
	$(LD) $(LDFLAGS) -T src/kernel/link.ld -o bin/kernel $(kernel-o) $(kernel-o32)
