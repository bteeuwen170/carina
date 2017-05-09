# Elarix

This is the README file for Elarix, a free and open source operating system
kernel for the i386 and x86\_64 architecture. I don't have a primary goal,
but I'm trying to make this OS as portable, extensible and versatile
as possible.

Do keep in mind that this is just an experimental operating system.
I've encountered many occasions where I tried to do things my own way - as
opposed to the Unix way for example - but in the end, I still ended up doing it
their way simply because you can't reinvent the wheel,
no matter how eager you may be.

License: GPLv2


## Building the Elarix toolchain

These utilities are required to build the Elarix toolchain:

* GNU C Compiler
* GNU make
* libtool

An active Internet connection is also needed to download the necessary files.

Issue this command to build the Elarix toolchain (note that you may be asked for
root permissions during installation):

	make toolchain


## Building Elarix

These utilities are required to build the Elarix kernel:

* Elarix toolchain
* GNU make
* cdrkit (for genisoimage to build an ISO image)
* GRUB (for grub-mkimage to build an ISO image)

Issue this command to build the elarix kernel:

	make elarix
