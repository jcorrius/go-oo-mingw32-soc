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

ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT
AC_PROG_INTLTOOL

AC_ARG_WITH(
	[ooo-solver-dir],
	AC_HELP_STRING([--with-ooo-solver-dir=DIR],
		[path to your OpenOffice.org solver]),
	[ac_ooodev_bindir="${withval}/bin"
	 ac_ooodev_includes="${withval}/inc"])

AC_ARG_WITH(
	[ooo-install-dir],
	AC_HELP_STRING([--with-ooo-install-dir=DIR],
		[path to your OpenOffice.org installation]),
	[ac_ooo_libdir="${withval}/program"])

PKG_CHECK_MODULES(OOO_BONOBO, gtk+-2.0 libgnome-2.0 libbonoboui-2.0)

ac_cppflags_safe="$CPPFLAGS"
CPPFLAGS="-I$ac_ooodev_includes"
AC_CHECK_HEADERS([sal/config.h],
		 [OOO_BONOBO_CFLAGS="$OOO_BONOBO_CFLAGS -I$ac_ooodev_includes"],
		 AC_MSG_ERROR([[OpenOffice.org include files not found, try '--with-ooo-solver-dir=/path/to/solver']]))
CPPFLAGS="$ac_cppflags_safe"

AC_PATH_PROG([REGMERGE], [regmerge], [], [$ac_ooodev_bindir:$PATH])
if test -z "$REGMERGE" ; then
  AC_MSG_ERROR([regmerge not found, try '--with-ooo-solver-dir=location_of_ODK'.])
fi
AC_SUBST(REGMERGE)

AC_PATH_PROG([REGCOMP], [regcomp], [], [$ac_ooodev_bindir:$PATH])
if test -z "$REGCOMP" ; then
  AC_MSG_ERROR([regcomp not found, try '--with-ooo-solver-dir=location_of_ODK'.])
fi
AC_SUBST(REGCOMP)

if test ! -d "$ac_ooo_libdir" ; then
	AC_MSG_ERROR([[OpenOffice.org installation not found. Please use --with-ooo-install-dir=/path/to/ooo-installation]])
fi
OOO_BONOBO_LIBS="$OOO_BONOBO_LIBS -L$ac_ooo_libdir"
OOO_libdir="$ac_ooo_libdir"
AC_SUBST(OOO_libdir)

dnl UDK Type Registry
AC_CHECK_FILE([$ac_ooodev_bindir/udkapi.rdb],
	      [DKREGISTRYNAME="$ac_ooodev_bindir/udkapi.rdb"],
	      AC_MSG_ERROR([[udkapi.rdb not found. Please use --with-ooo-install-dir=/path/to/ooo-installation]]))

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