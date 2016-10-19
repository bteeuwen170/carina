#
# toolchain/Makefile
#

# FIXME Not foolproofly written

# I shoud really use autoconf for this...

PHONY += toolchain
toolchain: toolchain-binutils toolchain-gcc


PHONY += toolchain-binutils
toolchain-binutils: toolchain/binutils-2.27

toolchain/binutils-2.27.tar.gz:
	@echo -e "\033[1;37m> Downloading GNU Binutils 2.27...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/binutils/binutils-2.27.tar.gz" -O toolchain/binutils-2.27.tar.gz

toolchain/binutils-2.27: toolchain/binutils-2.27.tar.gz
	@echo -e "\033[1;37m> Extracting GNU Binutils 2.27...\033[0m"
	@tar -xvf toolchain/binutils-2.27.tar.gz -C toolchain/
	@rm -f toolchain/binutils-2.27/ld/Makefile.in
	@echo -e "\033[1;37m> Patching GNU Binutils 2.27...\033[0m"
	@patch -p0 < toolchain/binutils-2.27.patch
	@cd toolchain/binutils-2.27/ld/ && automake
	@echo -e "\033[1;37m> Building GNU Binutils 2.27...\033[0m"
	@mkdir toolchain/binutils-2.27/build/
	@cd toolchain/binutils-2.27/build/ && ../configure --prefix=/usr/local --target=$(ARCH)-elf-clemence --disable-nls && make && sudo make install


PHONY += toolchain-gcc
toolchain-gcc: toolchain/gcc-4.9.4

toolchain/gcc-4.9.4.tar.gz: toolchain-binutils
	@echo -e "\033[1;37m> Downloading GCC 4.9.4...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/gcc/gcc-4.9.4/gcc-4.9.4.tar.gz" -O toolchain/gcc-4.9.4.tar.gz

toolchain/gcc-4.9.4: toolchain/gcc-4.9.4.tar.gz
	@echo -e "\033[1;37m> Extracting GCC 4.9.4...\033[0m"
	@tar -xvf toolchain/gcc-4.9.4.tar.gz -C toolchain/
	@echo -e "\033[1;37m> Patching GCC 4.9.4...\033[0m"
	@patch -p0 < toolchain/gcc-4.9.4.patch
	@cd toolchain/gcc-4.9.4/libstdc++-v3/ && autoconf
	@echo -e "\033[1;37m> Preparing GCC 4.9.4...\033[0m"
	@cd toolchain/gcc-4.9.4/ && ./contrib/download_prerequisites
	@ln -s ../binutils-2.27/build/bfd toolchain/gcc-4.9.4/bfd
	@ln -s ../binutils-2.27/build/binutils toolchain/gcc-4.9.4/binutils
	@ln -s ../binutils-2.27/build/gas toolchain/gcc-4.9.4/gas
	@ln -s ../binutils-2.27/build/gprof toolchain/gcc-4.9.4/gprof
	@ln -s ../binutils-2.27/build/opcodes toolchain/gcc-4.9.4/opcodes
	@echo -e "\033[1;37m> Building GCC 4.9.4...\033[0m"
	@mkdir toolchain/gcc-4.9.4/build/
	@cd toolchain/gcc-4.9.4/build/ && ../configure --prefix=/usr/local --target=$(ARCH)-elf-clemence --enable-languages=c --disable-bootstrap --without-headers --disable-nls --disable-multilib --disable-libssp && make all-gcc && make all-target-libgcc && sudo make install
