#!/bin/sh
# Run this to generate all the initial makefiles, etc.

aclocal
automake --gnu --add-missing
echo "no" | gettextize --force --copy
intltoolize --copy --force --automake
autoconf
./configure $@
