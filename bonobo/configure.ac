AC_PREREQ(2.53)
AC_INIT(src/main.cxx)

AM_INIT_AUTOMAKE(ooo-bonobo, 0.0.1)

AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AC_STDC_HEADERS
AM_PROG_LIBTOOL

GETTEXT_PACKAGE=ooo-bonobo
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE")
AC_SUBST(GETTEXT_PACKAGE)

ALL_LINGUAS="de"
AM_GLIB_GNU_GETTEXT
AC_PROG_INTLTOOL

AC_ARG_ENABLE(
	[strict-tests],
	AC_HELP_STRING([--enable-strict-tests],
		[check for the presence of headers, binaries, etc.]),,
	[enable_strict_tests="no"])

dnl FIXME
TAG=`$srcdir/../bin/latest-patchset $srcdir/../patches`;
BASEDIR=`pwd`/..

AC_ARG_WITH(
	[ooo-builddir],
	AC_HELP_STRING([--with-ooo-builddir],
		[define the directory where openoffice.org will be compiled, e.g. the root of an ooo cvs checkout.]),
	[OOBUILDDIR="${withval}"],
	[if test "z$with_src" = "z"; then
      	    OOBUILDDIR=$BASEDIR/build/$TAG
   	 else
      	    OOBUILDDIR=$with_src/$TAG
   	 fi])

ac_solver_dir="$OOBUILDDIR/solver/645/unxlngi4.pro"

AC_ARG_WITH(
	[ooo-solver-dir],
	AC_HELP_STRING([--with-ooo-solver-dir=DIR],
		[path to your OpenOffice.org solver]),
	[ac_solver_dir="${withval}"],)

ac_ooodev_bindir="${ac_solver_dir}/bin"
ac_ooodev_includes="${ac_solver_dir}/inc"
ac_ooodev_libdir="${ac_solver_dir}/lib"

AC_ARG_WITH(
	[ooo-install-dir],
	AC_HELP_STRING([--with-ooo-install-dir=DIR],
		[path to your OpenOffice.org installation]),
	[OOO_INSTALLDIR=${withval}],
	[OOO_INSTALLDIR=${libdir}/ooo-1.1])
AC_SUBST(OOO_INSTALLDIR)

PKG_CHECK_MODULES(OOO_BONOBO, gtk+-2.0 libgnome-2.0 libbonoboui-2.0)

ac_cppflags_safe="$CPPFLAGS"
CPPFLAGS="-I$ac_ooodev_includes"
AC_CHECK_HEADERS([sal/config.h],
		 [],
		 test "z$enable_strict_tests" = "zno" || AC_MSG_ERROR([[OpenOffice.org include files not found, try '--with-ooo-solver-dir=/path/to/solver']]))
OOO_BONOBO_CFLAGS="$OOO_BONOBO_CFLAGS -I$ac_ooodev_includes"
CPPFLAGS="$ac_cppflags_safe"

AC_PATH_PROG([REGMERGE], [regmerge], [], [$ac_ooodev_bindir:$PATH])
if test -z "$REGMERGE" ; then
  if test "z$enable_strict_tests" = "zno"; then
    REGMERGE="$ac_ooodev_bindir/regmerge"
  else
    AC_MSG_ERROR([regmerge not found, try '--with-ooo-solver-dir=location_of_ODK'.])
  fi
fi
dnl FIXME
REGMERGE="LD_LIBRARY_PATH=$ac_ooodev_libdir:\$\$LD_LIBRARY_PATH $REGMERGE"
AC_SUBST(REGMERGE)

AC_PATH_PROG([REGCOMP], [regcomp], [], [$ac_ooodev_bindir:$PATH])
if test -z "$REGCOMP" ; then
  if test "z$enable_strict_tests" = "zno"; then
    REGCOMP="$ac_ooodev_bindir/regcomp"
  else
    AC_MSG_ERROR([regcomp not found, try '--with-ooo-solver-dir=location_of_ODK'.])
  fi
fi
dnl FIXME
REGCOMP="LD_LIBRARY_PATH=$ac_ooodev_libdir:\$\$LD_LIBRARY_PATH $REGCOMP"
AC_SUBST(REGCOMP)

if test ! -d "$ac_ooodev_libdir" && test "z$enable_strict_tests" != "zno" ; then
  AC_MSG_ERROR([[OpenOffice.org libraries not found. Please use --with-ooo-solver-dir=/path/to/solver]])
fi
OOO_BONOBO_LIBS="$OOO_BONOBO_LIBS -L$ac_ooodev_libdir"
OOODEV_LIBDIR="$ac_ooodev_libdir"
AC_SUBST(OOODEV_LIBDIR)

dnl Type Registry (types.rdb = offapi + udkapi)
AC_CHECK_FILE([$ac_ooodev_bindir/types.rdb],
	      [],
	      test "z$enable_strict_tests" = "zno" || AC_MSG_ERROR([[udkapi.rdb not found. Please use --with-ooo-install-dir=/path/to/ooo-installation]]))

DKREGISTRYNAME="$ac_ooodev_bindir/types.rdb"
AC_SUBST(DKREGISTRYNAME)

dnl FIXME
GUI=UNX
COM=GCC
OS=LINUX
CPPU_ENV=gcc3

OOO_BONOBO_CFLAGS="$OOO_BONOBO_CFLAGS -D$GUI -D$COM -D$OS -DCPPU_ENV=$CPPU_ENV"

dnl FIXME
CPPUHELPERLIB=-lcppuhelpergcc3
CPPULIB=-lcppu
SALHELPERLIB=-lsalhelpergcc3
SALLIB=-lsal
STLPORTLIB=-lstlport_gcc

OOO_BONOBO_LIBS="$OOO_BONOBO_LIBS $CPPUHELPERLIB $CPPULIB $SALHELPERLIB $SALLIB $STLPORTLIB"

AC_SUBST(OOO_BONOBO_CFLAGS)
AC_SUBST(OOO_BONOBO_LIBS)

AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
po/Makefile.in
])
