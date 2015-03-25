About
=====

This library approximates the behavior of the now obsolete
POSIX ftw() and it's successor nftw().  Some nftw() analogs
are implemented and some are unfisihed.  Unfortunately, there will
never be compelete parity due to the intentional behavior of xftw().
herever possible, ntfw() behavior is preserved.


Rationale
=========

1) Some implmentations of ftw() and ntfw() rely on fts(3).  FTS
   is incompatible with _FILE_OFFSET_BITS = 64 on GNU libc systems.

2) Unlike ftw() and ntfw(), xftw() allows the user to provide a data
   pointer that will passed down to the iterator callback.  This is
   quite useful when you need relay information down to the iterator
   layer and perform unique actions.


License
=======

This software is copyright 2015 Bryan Christ and can redistributed
and/or modified under the terms of the GNU General Public License version 2.

For the exact terms of the license, see `LICENSE`.

Compiling
=========

On Debian and derivatives like Ubuntu you need the following packages to build
this software:

	apt-get install cmake build-essential 

On Red Hat and derivatives like Fedora you need the following packages to build
this software:

	yum groupinstall "Development Tools" "Development Libraries"
	yum install cmake 

On FreeBSD you need:

	pkg install cmake 

This project is using cmake, so you can build the software out-of-tree:

	mkdir build
	cd build
	cmake ..
	make
	make install

Bugs
====

If you encounter a bug, please file a ticket with sample source to 
reproduce it.  Even better, try and fix it and submit a pull request.
