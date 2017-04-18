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

	/                       root
	├── app                 applications
	│   ├── bin             application binaries
	│   ├── cfg             application configuration files
	│   ├── inc             c include files
	│   ├── lib             shared libraries
	│   ├── shr             application data
	│   └── tmp             application cache
	├── boot                boot loader files
	│   └── grub            GRUB configuration
	├── cfg                 system configuration files
	├── mnt                 mounting points
	├── sys                 kernel files (ramfs)
	│   ├── dev             device files
	│   ├── lck             lock files
	│   ├── krn             kernel files
	│   └── prc             process information
	├── tmp                 temporary files (ramfs)
	└── usr                 users home directories
	    └── [user]          user home directory
	        ├── .app        user-specific applications folder
	        │   ├── bin     user-specific application binaries
	        │   ├── cfg     user-specific application configuration files
	        │   ├── inc     user-specific C include files
	        │   ├── lib     user-specific shared libraries
	        │   ├── shr     user-specific application data
	        │   └── tmp     user-specific application cache
	        ├── Documents   user documents
	        ├── Downloads   user downloads
	        ├── Music       user music
	        ├── Pictures    user pictures
	        └── Videos      user videos

## cmdline

The following cmdline options are supported:

	console=                specify the system console
