# Elarix

This is the README file for Elarix, a free and open source operating system
kernel for the i386 and x86\_64 architecture.

License: GPLv2


## Building the toolchain

These utilities are required to build the Elarix toolchain:

* GNU C Compiler
* GNU make
* libtool

An active Internet connection is also needed to download the necessary files.

Issue this command to build the Elarix toolchain:

	make toolchain


## Building Elarix

These applications are required to build the Elarix kernel:

* Elarix toolchain
* GNU make
* cdrkit (for genisoimage to build an ISO image)
* GRUB (for grub-mkimage to build an ISO image)

Issue this command to build the elarix kernel:

	make elarix
