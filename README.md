# Carina

This is the README file for Carina, a free and open source operating system for
the i686 and x86\_64 architecture.

License: GPLv2


## File system tree
Carina does not follow the FHS standard, instead it uses this
filesystem hierarchy layout:

	/                       Root
	├── app                 Applications
	│   ├── bin             Application binaries
	│   ├── cfg             Application configuration files
	│   ├── inc             C include files
	│   ├── lib             Shared libraries
	│   ├── shr             Application data
	│   └── tmp             Application cache
	├── boot                Boot loader files
	│   └── grub            GRUB configuration
	├── cfg                 System configuration files
	├── dev                 Device files (devfs)
	├── usr                 Users home directories
	│   └── [user]          User home directory
	│       ├── .app        User-specific applications folder
	│       │   ├── bin     User-specific application binaries
	│       │   ├── cfg     User-specific application configuration files
	│       │   ├── inc     User-specific C include files
	│       │   ├── lib     User-specific shared libraries
	│       │   ├── shr     User-specific application data
	│       │   └── tmp     User-specific application cache
	│       ├── Documents   User documents
	│       ├── Downloads   User downloads
	│       ├── Music       User music
	│       ├── Pictures    User pictures
	│       └── Videos      User videos
	├── sys                 Kernel files (sysfs)
	│   ├── lck             Lock files
	│   ├── krn             Kernel files
	│   ├── prc             Process information
	│   └── TODO
	└── tmp                 Temporary files (tmpfs)


## Building the toolchain

These utilities are required to build the Carina toolchain:

* GNU C Compiler (or Clang)
* GNU autoconf 2.64
* GNU automake 2.11.6
* GNU make
* libtool

An active Internet connection is also needed to download the necessary files.

Issue this command to build the Carina toolchain:

	make toolchain


## Building Carina

These applications are required to build the Carina kernel:
* GNU make
* Carina toolchain
* cdrkit (for genisoimage to build an ISO image)
* GRUB (for grub-mkimage to build an ISO image)

Issue this command to build the carina kernel:

	make carina


## About

Please refer to [my website](https://teeuwen.github.io).
