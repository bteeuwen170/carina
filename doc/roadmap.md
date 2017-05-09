# Roadmap for Elarix

* Architectures
	* i386...
	* i686...
	* ~~x86_64~~
* Drivers
	* ~~Global driver interface~~
	* ~~Global device interface~~
	* ACPI
	* Block device drivers
		* AHCI
		* Block device interface (with buffering)...
		* IDE
			* ATA
				* Reading
				* Writing
			* ATAPI
				* ~~Reading~~
				* Ejecting
	* Bus drivers
		* ~~PCI~~
		* PS/2...
	* Character device drivers
		* AT keyboard...
	* Networking
		* TCP/IP
			* Ethernet
			* TCP
			* UDP
	* Real Time Clocks
		* ~~CMOS~~
	* Sound
		* AC97
			* Playback...
		* SoundBlaster 16...
	* System timers
		* ~~Timer interface~~
		* ~~PIT~~
	* Video device
		* Console
			* Serial...
			* ~~VGA~~
		* VESA
* File System
	* Standardized and researched file system layout (not FHS)...
	* VFS...
		* ~~Mounting~~
		* Unmounting... (nearly done)
		* ~~Inode caching~~
		* ~~Dirent caching~~
	* Reading ELF...
	* ~~devfs~~
	* ext2
		* Reading
		* Writing
	* vfat(12/16/32)
		* Reading
		* Writing
	* ~~iso9660~~
		* Rockridge
	* memfs...
	* sysfs
* Inter Process Communication
	* Pipes
	* Signals
* klibc...
* Memory Management
	* Proper paging...
	* Higher half kernel...
	* Physical memory manager
	* Virtual memory manager
	* kmalloc...
* Multitasking
	* Preemptive scheduling
* Userspace
	* System calls
	* libc

<sub<b>Notes</b></sub>
* <sub>Three dots indicate that this goal is currently being worked on.</sub>
* <sub>Strikethrough indicates that this goals has been completed.</sub>
