#!/bin/sh
# Run this to generate all the initial makefiles, etc.

if test "z$1" == "z--clean"; then
    echo "Cleaning"

    rm -Rf autom4te.cache bonobo/autom4te.cache
    rm -f missing install-sh mkinstalldirs libtool ltmain.sh
    rm -f bonobo/missing bonobo/install-sh bonobo/mkinstalldirs \
	bonobo/libtool bonobo/ltmain.sh
    exit 1;
fi

(cd bonobo && ./autogen.sh)
echo "NOT YET."

aclocal
automake --gnu --add-missing
# intltoolize --copy --force --automake
autoconf
./configure $@
