#!/bin/sh
# Run this to generate all the initial makefiles, etc.

if test "z$1" = "z--clean"; then
    echo "Cleaning"

    rm -Rf autom4te.cache bonobo/autom4te.cache
    rm -f missing install-sh mkinstalldirs libtool ltmain.sh
    rm -f bonobo/missing bonobo/install-sh bonobo/mkinstalldirs \
	bonobo/libtool bonobo/ltmain.sh
    exit 1;
fi

aclocal $ACLOCAL_FLAGS || exit 1;
automake --gnu --add-missing --copy || exit 1;
# intltoolize --copy --force --automake
autoconf || exit 1;
if test "x$NOCONFIGURE" = "x"; then
    ./configure "$@"
else
    echo "Skipping configure process."
fi
