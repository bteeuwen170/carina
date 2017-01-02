#
# util/menuconfig/Makefile
#

menuconfig-o += menu.o menuconfig.o parser.o

%.o: %.c
	echo -e "  HOSTCC  $@"
	$(HOSTCC) $(HOSTCFLAGS) -c $< -o $@

PHONY += menuconfig
menuconfig: $(menuconfig-o)
	echo -e "  HOSTCC  $@"
	$(HOSTCC) -lncurses -o menuconfig $(menuconfig-o)
