#!/bin/sh
# Run this to generate all the initial makefiles, etc.

(cd bonobo && ./autogen.sh)

aclocal
automake --gnu --add-missing
# intltoolize --copy --force --automake
autoconf
./configure $@
