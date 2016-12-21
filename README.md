# Elara

This is the README file for Elara, a free and open source operating system
kernel for the i686 and x86\_64 architecture.

License: GPLv2


## Building the toolchain

These utilities are required to build the Elara toolchain:

* GNU C Compiler (or Clang)
* GNU autoconf 2.64
* GNU automake 2.11.6
* GNU make
* libtool

An active Internet connection is also needed to download the necessary files.

Issue this command to build the Elara toolchain:

	make toolchain


## Building Elara

These applications are required to build the Elara kernel:
* GNU make
* Elara toolchain
* cdrkit (for genisoimage to build an ISO image)
* GRUB (for grub-mkimage to build an ISO image)

Issue this command to build the elara kernel:

	make elara


## File system tree
Elara does not adhere to the FHS standard, instead it uses this
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
	├── mnt                 Mounting points
	├── sys                 Kernel files (sysfs)
	│   ├── dev             Device files (devfs)
	│   ├── lck             Lock files
	│   ├── krn             Kernel files
	│   └── prc             Process information
	├── tmp                 Temporary files (tmpfs)
	└── usr                 Users home directories
	    └── [user]          User home directory
	        ├── .app        User-specific applications folder
	        │   ├── bin     User-specific application binaries
	        │   ├── cfg     User-specific application configuration files
	        │   ├── inc     User-specific C include files
	        │   ├── lib     User-specific shared libraries
	        │   ├── shr     User-specific application data
	        │   └── tmp     User-specific application cache
	        ├── Documents   User documents
	        ├── Downloads   User downloads
	        ├── Music       User music
	        ├── Pictures    User pictures
	        └── Videos      User videos


## About

Please refer to [my website](https://teeuwen.github.io/projects/elara).
