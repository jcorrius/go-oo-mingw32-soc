#!/bin/sh
# Run this to generate all the initial makefiles, etc.

aclocal
automake --gnu
autoconf
./configure $@
