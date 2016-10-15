#
# toolchain/Makefile
#

PHONY += toolchain
toolchain: toolchain-gcc toolchain-binutils

toolchain-gcc: toolchain/gcc.tar.gz
toolchain/gcc.tar.gz:
	@echo -e "\033[1;37m> Downloading GNU C Compiler 4.9.4...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/gcc/gcc-4.9.4/gcc-4.9.4.tar.gz" -O toolchain/gcc.tar.gz

toolchain-binutils: toolchain/binutils.tar.gz
toolchain/binutils.tar.gz:
	@echo -e "\033[1;37m> Downloading GNU Binutils 2.27...\033[0m"
	@wget $(WGETFLAGS) "https://ftp.gnu.org/gnu/binutils/binutils-2.27.tar.gz" -O toolchain/binutils.tar.gz

#toolchain-libc:
#	@echo -e "\033[1;37m> Downloading newlib...\033[0m"
#	@wget $(WGETFLAGS) ""
