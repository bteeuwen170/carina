#
# toolchain/Makefile
#

# FIXME Not foolproofly written
# FIXME Unnecessary redownloading of source files

#ASFLAGS = $(HOSTASFLAGS)
#CFLAGS = $(HOSTCFLAGS)
#LDFLAGS = $(HOSTLDFLAGS)
#export ASFLAGS CFLAGS LDFLAGS

# I shoud really use autoconf for this...

export USE_AUTOCONF_VER = 2.64
export AUTOCONF		= $(CURDIR)/toolchain/local/bin/autoconf
export USE_AUTOMAKE_VER	= 1.11.6
export AUTOMAKE		= $(CURDIR)/toolchain/local/bin/automake

PHONY += toolchain
toolchain: toolchain-gcc


PHONY += toolchain-autoconf
toolchain-autoconf: toolchain/autoconf-2.64

toolchain/autoconf-2.64.tar.gz:
	@echo -e "\033[1;37m> Downloading GNU Autoconf 2.64...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.gz" -O toolchain/autoconf-2.64.tar.gz

toolchain/autoconf-2.64: toolchain/autoconf-2.64.tar.gz
	@echo -e "\033[1;37m> Extracting GNU Autoconf 2.64...\033[0m"
	@tar -xvf toolchain/autoconf-2.64.tar.gz -C toolchain/
	@echo -e "\033[1;37m> Building GNU Autoconf 2.64...\033[0m"
	@mkdir -p toolchain/local
	@cd toolchain/autoconf-2.64 && ./configure --prefix="$(CURDIR)/toolchain/local" && make
	@echo -e "\033[1;37m> Installing GNU Autoconf 2.64...\033[0m"
	@cd toolchain/autoconf-2.64 && make install


PHONY += toolchain-automake
toolchain-automake: toolchain/automake-1.11.6

toolchain/automake-1.11.6.tar.gz: toolchain-autoconf
	@echo -e "\033[1;37m> Downloading GNU Automake 1.11.6...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/automake/automake-1.11.6.tar.gz" -O toolchain/automake-1.11.6.tar.gz

toolchain/automake-1.11.6: toolchain/automake-1.11.6.tar.gz
	@echo -e "\033[1;37m> Extracting GNU Automake 1.11.6...\033[0m"
	@tar -xvf toolchain/automake-1.11.6.tar.gz -C toolchain/
	@echo -e "\033[1;37m> Building GNU Automake 1.11.6...\033[0m"
	@mkdir -p toolchain/local
	@cd toolchain/automake-1.11.6 && ./configure --prefix="$(CURDIR)/toolchain/local" && make
	@echo -e "\033[1;37m> Installing GNU Automake 1.11.6...\033[0m"
	@cd toolchain/automake-1.11.6 && make install


PHONY += toolchain-binutils
toolchain-binutils: toolchain/binutils-2.27

toolchain/binutils-2.27.tar.gz: toolchain-autoconf toolchain-automake
	@echo -e "\033[1;37m> Downloading GNU Binutils 2.27...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/binutils/binutils-2.27.tar.gz" -O toolchain/binutils-2.27.tar.gz

toolchain/binutils-2.27: toolchain/binutils-2.27.tar.gz
	@echo -e "\033[1;37m> Extracting GNU Binutils 2.27...\033[0m"
	@tar -xvf toolchain/binutils-2.27.tar.gz -C toolchain/
	@echo -e "\033[1;37m> Patching GNU Binutils 2.27...\033[0m"
	@patch -p0 < toolchain/binutils-2.27.patch
	@rm -f toolchain/binutils-2.27/ld/Makefile.in
	@cd toolchain/binutils-2.27/ld/ && $$AUTOMAKE
	@echo -e "\033[1;37m> Building GNU Binutils 2.27...\033[0m"
	@mkdir toolchain/binutils-2.27/build/
	@cd toolchain/binutils-2.27/build/ && ../configure --prefix=/usr/local --target=$(ARCH)-elf-elarix --disable-nls && make
	@echo -e "\033[1;37m> Requesting permission to install GNU Binutils 2.27...\033[0m"
	@cd toolchain/binutils-2.27/build/ && sudo make install


PHONY += toolchain-gcc
toolchain-gcc: toolchain/gcc-4.9.4

toolchain/gcc-4.9.4.tar.gz: toolchain-automake toolchain-autoconf toolchain-binutils
	@echo -e "\033[1;37m> Downloading GCC 4.9.4...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/gcc/gcc-4.9.4/gcc-4.9.4.tar.gz" -O toolchain/gcc-4.9.4.tar.gz

toolchain/gcc-4.9.4: toolchain/gcc-4.9.4.tar.gz
	@echo -e "\033[1;37m> Extracting GCC 4.9.4...\033[0m"
	@tar -xvf toolchain/gcc-4.9.4.tar.gz -C toolchain/
	@echo -e "\033[1;37m> Patching GCC 4.9.4...\033[0m"
	@patch -p0 < toolchain/gcc-4.9.4.patch
	@cd toolchain/gcc-4.9.4/libstdc++-v3/ && $$AUTOCONF
	@echo -e "\033[1;37m> Preparing GCC 4.9.4...\033[0m"
	@cd toolchain/gcc-4.9.4/ && ./contrib/download_prerequisites
	@ln -s ../binutils-2.27/build/bfd toolchain/gcc-4.9.4/bfd
	@ln -s ../binutils-2.27/build/binutils toolchain/gcc-4.9.4/binutils
	@ln -s ../binutils-2.27/build/gas toolchain/gcc-4.9.4/gas
	@ln -s ../binutils-2.27/build/gprof toolchain/gcc-4.9.4/gprof
	@ln -s ../binutils-2.27/build/opcodes toolchain/gcc-4.9.4/opcodes
	@echo -e "\033[1;37m> Building GCC 4.9.4...\033[0m"
	@mkdir toolchain/gcc-4.9.4/build/
	@cd toolchain/gcc-4.9.4/build/ && ../configure --prefix=/usr/local --target=$(ARCH)-elf-elarix --enable-languages=c --disable-bootstrap --without-headers --disable-nls --disable-multilib --disable-libssp --disable-libquadmath && make all-gcc && make all-target-libgcc
	@echo -e "\033[1;37m> Requesting permission to install GCC 4.9.4...\033[0m"
	@cd toolchain/gcc-4.9.4/build/ && sudo make install
