# Writing a driver for Elarix

WARNING: The device model in Elarix is extemely experimental and changes may and
WILL happen.

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

The device structure consists of the following items:

	struct device {
		struct list_head l;

		const char	*name;
		dev_t		dev;
		u8		flags;

		void		*bus;
		void		*device;

		struct driver *drip;

		struct file_ops *op;
	};

__dev__, __bus__ and __drip__ are automatically filled by
the device manager. These fields are therefore recommended not to be modified as
this may result in undefined behavior. Fields that can (and should) be modified,
preferably during the __*devname*\_probe__ routine are __name__, __flags__,
__device__ and __op__.

The __name__ field will be either "Generic device" by default (if the the device
hasn't been initialized by a bus driver first. It is entirely optional to use
this field but it is highly recommended to make the device more identifiable.

__dev__ simply contains the device ID of your device. Devices are identified by
their major number. As opposed to other operating system kernels, minors are
actually managed by the device manager instead of the driver.
Major numbers can be extracted from a dev_t using the __MAJOR()__ macro,
the same goes for minors (__MINOR()__).

__flags__ can be written to, but you should not overwrite any values that it
may already contain. The bus driver may have already set a couple of flags.
A list of flags can be found in the dev.h file.

__bus__ is completely bus specific and probably contains a structure with
information about the controller/device.
In the case of pci for example, this field contains the PCI Configuration Space
structure. To read such a structure, you should include you bus's header file.

It is 100% up to your driver what is placed in the __device__ field, in most
cases this will be a structure containing more information about the device.

__drip__ is just a pointer to the associated driver.

__op__ is a structure that is defined in fs.h. This field is set to NULL and
your driver is supposed to define a (preferably) constant structure containing
all the methods that your driver supports. Leaving this field empty is just
stupid as this is the only way that other drivers (besides calling your
functions direcly which is highly discouraged) and userspace will interface
with your driver.


## *devname*\_init


## *devname*\_exit


## *devname*\_probe

Next up is __*devname*\_probe__, this function is called once all modules have
been loaded and all buses have been initialized. It is a good idea to set
variables in the device structure correctly here and initialized the device
itself if required.
The device manager only sets a


## *devname*\_fini

Once a device is no longer needed by the system, this function will be called.
Here is a good place to finalize your device. Finalizing a display device would
mean freeing buffers and clearing the screen for example.


## Controllers
