# Elarix

This is the README file for Elarix, a free and open source operating system
kernel for the i386 and x86\_64 architecture.

License: GPLv2


## Building the toolchain

These utilities are required to build the Elarix toolchain:

* GNU C Compiler (or Clang)
* GNU autoconf 2.64
* GNU automake 2.11.6
* GNU make
* libtool

An active Internet connection is also needed to download the necessary files.

Issue this command to build the Elarix toolchain:

	make toolchain


## Building Elarix

These applications are required to build the Elarix kernel:
* GNU make
* Elarix toolchain
* cdrkit (for genisoimage to build an ISO image)
* GRUB (for grub-mkimage to build an ISO image)

Issue this command to build the elarix kernel:

	make elarix


## File system tree
Elarix does not adhere to the FHS standard, instead it uses this
filesystem hierarchy layout:

	/                       Root
	├── app                 Applications
	│   ├── bin             Application binaries
	│   ├── cfg             Application configuration files
	│   ├── inc             C include files
	│   ├── lib             Shared libraries
	│   ├── shr             Application data
	│   └── tmp             Application cache
	├── boot                Boot loader files
	│   └── grub            GRUB configuration
	├── cfg                 System configuration files
	├── mnt                 Mounting points
	├── sys                 Kernel files
	│   ├── dev             Device files (ramfs)
	│   ├── lck             Lock files (ramfs)
	│   ├── krn             Kernel files (ramfs)
	│   └── prc             Process information (ramfs)
	├── tmp                 Temporary files (ramfs)
	└── usr                 Users home directories
	    └── [user]          User home directory
	        ├── .app        User-specific applications folder
	        │   ├── bin     User-specific application binaries
	        │   ├── cfg     User-specific application configuration files
	        │   ├── inc     User-specific C include files
	        │   ├── lib     User-specific shared libraries
	        │   ├── shr     User-specific application data
	        │   └── tmp     User-specific application cache
	        ├── Documents   User documents
	        ├── Downloads   User downloads
	        ├── Music       User music
	        ├── Pictures    User pictures
	        └── Videos      User videos
