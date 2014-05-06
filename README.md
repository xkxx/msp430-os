Duckus: an operating system for the least powerful of machines
=========

Duckus walks like an operating system, quarks like an operating system, but you'd be a fool to think it is one. Which is its sole purpose: to support as many Unix console commands as we can cram into its stomach so that it appears like an operating system to those unaware.

It is created as an experiment to discover the extent as to how much bloat one can feed to the least performant hardware money can buy [Citation Needed] that is the TI MSP430 Launchpad.

Duckus IS NOT AN OS
---------------------

The TI MSP430 Launchpad has a maximum of 16KB flash memory and 512 Byte RAM. There is no way a modern OS with a gazillion features and all the bloat can fit on it. Neither can an embedded OS with memory management, multitasking, file system, IO management, and command line jumbo can fit on it.

Thus comes Duckus: it's not an operating system, nor does it try to be. But it _pretend_ to be an operating system. Once _booted_ up, it presents you with a very nice command shell that is similar to any typical Unix command shell, and supports a good chunk of frequently used Unix commands. They include:

* yes
* uname
* pwd
* echo
* cd
* cat
* ln
* ls
* mkdir
* dc
* sh
* _more to come..._

And a few commands only present on MSP430OS, so that you can actually do something with the Launchpad hardware:

* red (controls the red light)
* green

It also emulates a file system that you can play with, like traversing directories, manipulating files, etc.

Installation
------------

The .ccsproject file is included for easy import into the TI Code Composer Studio. Load it up, and you are good to go. If it's a new project in CCS, make sure you have enabled `Relaxed parsing(non-strict ANSI)` in language options. If you use MSP430GCC, you may have to modify the interrupt functions in `std.c` a bit, as the syntax are incompatible. After compilation, load the code via CCS or msp430debug, then open up a serial console of your choice (putty, screen, or minicom), connect to the serial port(COM* or /dev/ttyACM*), and Duckus should be there await for your commands!

Portability
-----------

The majority of the source code is platform-independent. You can easily port it over to run on a desktop computer, for example (it's also something Duckus will soon support natively). If you would like to port it over to another equally limited hardware platform, like Attiny, you may need to write your own initialization code and interrupt handlers.

Implementation
--------------

Such as the nature of the experiment, every component of the MSP430OS is homemade from the ground up. I tried to use the BSP430 library, but it is too big to fit the memory. Duckus does not use the native C library. It has its own overflow-safe string type, and some essential string manipulation functions like `streq` and `tokenize` (Like `strtok` but safer). Even `malloc()` and `free()` are rewritten to use a 150 byte static memory pool rather than the heap. Everything is shrinked down to save memory, esp. the precious RAM. _Developing..._
