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

for arg in "$@"; do
    case "$arg" in
	*' '*)

	    echo '

This autogen.sh script does not handle command-line arguments
containing spaces correctly. So if you need to pass such arguments on
to the configure script that autogen.sh produces, you should run
autogen.sh first without arguments, then run ./configure with the
arguments you want.'

	    exit 1
	    ;;
    esac
done

old_args=""
if test $# -eq 0 && test -f config.log; then
    old_args=`grep '\$ ./configure' config.log | sed -e 's/.*configure //' -e 's/=\(\([^"'"'"'-]\|-[^-]\| \)*\)\( \|$\)/="\1" /g'`
    echo "re-using arguments from last configure: $old_args";
fi

aclocal $ACLOCAL_FLAGS || exit 1;
automake --gnu --add-missing --copy || exit 1;
# intltoolize --copy --force --automake
autoconf || exit 1;
if test "x$NOCONFIGURE" = "x"; then
    eval `echo ./configure "$@" $old_args`
else
    echo "Skipping configure process."
fi
