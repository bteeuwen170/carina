## File system tree

Elarix does not adhere to the FHS standard, instead it uses this
file system hierarchy layout:

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
	├── sys                 kernel files
	│   ├── dev             device files (devfs)
	│   ├── lck             lock files (lckfs)
	│   ├── krn             kernel files (krnfs)
	│   └── prc             process information (prcfs)
	├── tmp                 temporary files (memfs)
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
