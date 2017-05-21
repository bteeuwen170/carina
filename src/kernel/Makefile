#
# src/kernel/Makefile
#

include src/kernel/arch/$(ARCHT)/Makefile
include src/kernel/bin/Makefile
include src/kernel/dev/Makefile
include src/kernel/fs/Makefile
include src/kernel/kernel/Makefile
include src/kernel/lib/Makefile
include src/kernel/mm/Makefile

kernel-i += -I src/kernel/dev
kernel-i += -I src/kernel/include
kernel-i += -I src/kernel/lib

TYPES = src/kernel/include/sys/types.h

%.o: %.c
	echo -e "  CC      $<"
	$(CC) $(CFLAGS) -include $(TYPES) -c $< $(kernel-i) -o $@

%.o: %.S
	echo -e "  AS      $<"
	$(CC) $(ASFLAGS) -c $< -o $@


PHONY += kernel
kernel: bin/kernel
bin/kernel: $(kernel-o)
	echo -e "  LD      $<"
ifeq ($(ARCH),x86_64)
	$(LD) $(LDFLAGS) -T src/kernel/arch/$(ARCHT)/boot/link-64.ld -o bin/kernel $(kernel-o) -lgcc
else
	$(LD) $(LDFLAGS) -T src/kernel/arch/$(ARCHT)/boot/link-32.ld -o bin/kernel $(kernel-o) -lgcc
endif
