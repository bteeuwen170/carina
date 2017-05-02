# Writing a driver for Elarix

Everything you need to work with drivers on Elarix is included in the "dev.h"
header found in "src/kernel/include/dev.h".

To create a driver, you need at least 4 functions, a driver structure and a
variable called __devname__:

	static const char devname[] = "[devname]";

Replace *[devname]* with the name of your driver. This definition can be omitted
in the case that you are not planning on using dprintf, which uses devname
internally to prefix in the kernel logs. We'll refer to the name of your driver
using *devname* from here on.

## The driver structure


## The device structure


## *devname*\_init


## *devname*\_exit


## *devname*\_probe

Next up is __*devname*\_probe__, this function is called once all modules have
been loaded and all buses have been initialized. It is a good idea to set
variables in the device structure correctly here.
The device manager only sets


## *devname*\_fini


## Controllers
