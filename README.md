MSP430OS
=========

MSP430OS walks like an operating system, quarks like an operating system, but is not one. But it tries to support as many Unix console commands as the memory can hold to make it seem like one to an unaware observer.

It is created as an experiment to discover the extent to how much we can do on a very limited hardware environment that is the TI MSP430 Launchpad.

MSP430OS IS NOT AN OS
---------------------

The TI MSP430 Launchpad has a maximum of 16KB flash memory and 512 Byte RAM. There is no way a modern OS with a gazillion features and all the bloat can fit on it. There is no way an embedded OS with memory management, multitasking, file system, IO management, and command line interface can fit on it.

Thus the MSP430OS is born: it's not an operating system, nor does it try to be. But it _pretend_ to be an operating system. Once _booted_ up, it presents you with a very nice command shell that is similar to any typical Unix command shell, and supports a good chunk of frequently used Unix commands. They include:

* yes
* uname
* pwd
* cd
* ls
* mkdir
* sh
* _more to come..._

And a few commands only present on MSP430OS, so that you can actually do something with the Launchpad hardware:

* red (controls the red light)
* green

It also emulates a file system that you can play with, traversing directories, manipulating files, etc.

Installation
------------

The .ccsproject file is included for easy import into the TI Code Composer Studio. Load it up, and you are good to go. If it's a new project in CCS, make sure you have enabled `Relaxed parsing(non-strict ANSI)` in language options. If you use MSP430GCC, you may have to modify the interrupt functions in `std.c` a bit, as the syntax are incompatible. After compilation, load the code via CCS or msp430debug, then open up a serial console of your choice (putty, screen, or minicom), connect to the serial port(COM* or /dev/ttyACM*), and the command prompt should be there await for your commands!

Implementation
--------------

Such as the nature of the experiment, every component of the MSP430OS is homemade from the ground up. I tried to use the BSP430 library, but it is too big to fit the memory. The standard C string library is used, but that's about it. Even `malloc()` and `free()` are rewritten to use a 150 byte static memory pool rather than the heap. Everything is shrinked down to save memory, esp. the precious RAM. _Developing..._
