NOTE: This README file is likely very outdated.  Please forward any questions
to the dev@sc.openoffice.org list.  Thanks.

Hello,

In this document, I will try to give you some overview of this source code to 
hopefully help you get started.

First, this package is only tested on a Linux platform, more specifically on
Fedora Core 3 platform, but it should not be too difficult to make it to
work under different platforms since I only use those libraries that
are available across all major (and minor) platforms.  It makes use of
the standard C++ library and the boost library, so you will need to
install that first if you don't have the boost library already installed on
your system.  To build the sources, you need to have GNU Make, or something
compatible with it.

To compile the source, first you need to edit the "setup.mk" file to fit your
environment.  This assumes that you already have a compiled OO.o source
files (680 codebase) located somewhere on your computer with the base
directory named after the cvs tag (e.g. SRC680_m106), *and* the binaries
built from the sources installed on the same system.  If this is the case,
you only need to edit the following 6 variables:

  CODEBASE - should be 680 for the 2.0-base code.
  MILESTONE - the milestone of the code base (the latest as of this writing
    is 111)
  BASESRCDIR - the parent directory of the base directory of the OO.o
    source code
  BASEAPPDIR - the parent directory of the base application directory
  PRODIRNAME - the name of the product directory for each module where all
  compiled stuff is thrown in (for x86 Linux it's either unxlngi4.pro
    or unxlngi6.pro).

For example, in my environment, the sources (as of this writing,
its SRC680_m106) are located under /mnt/ooo/SRC680_m106, and the binaries
under /opt/openoffice.org1.9.106, so the values in my setup.mk are set up
accordingly (see enclosed setup.mk for my values).

If you decide to use the SDK instead of the source code, you can ignore

  CODEBASE
  BASESRCDIR
  PRODIRNAME
  SRCDIR

Instead, set PRJ to the base directory of your SDK.  Note that I have
personally not tested this code against the SDK, so it is possible that I
may be missing out something.

I recommend that you use a visibility-enabled GCC compiler to compile
the sources, as it make the compiled binary significantly smaller.

For any questions and comments, please ask me on dev@sc.openoffice.org
mailing list.  I'll answer whatever questions you may have about this
code within my capacity.  If your question is outside my capacity, I'll
simply forward it to whoever might provide an answer for it.


All the best,

Kohei

Kohei Yoshida <kohei.yoshida@gmail.com>
