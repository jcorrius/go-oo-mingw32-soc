AC_INIT(scsolver.uno, 0.1, kyoshida@novell.com)

AC_ARG_WITH(ooo,
[
  --with-ooo              specify the base directory path of OO.o installation.
			  
			  Example:  --with-ooo=/opt/openoffice.org2.3],
,)

AC_ARG_WITH(ooo-sdk,
[
  --with-ooo-sdk          specify the base directory path of OO.o SDK.
			  
			  Example:  --with-ooo-sdk=/opt/openoffice.org2.3_SDK],
,)

AC_ARG_WITH(lpsolve,
[
  --with-lpsolve          specify the base directory path of lpsolve.
			  
			  Example:  --with-lpsolve=./lpsolve],
,)

AC_ARG_WITH(platform,
[
  --with-platform         specify the platform to build the package on.
  			  select from (Linux_x86, ...)

                       	  Example: --with-platform=Linux_x86],
,)

SCSOLVERDIR=../..
AC_SUBST(SCSOLVERDIR)

OBJDIR=./obj
AC_SUBST(OBJDIR)

PLATFORM=$with_platform
AC_MSG_CHECKING([checking platform])
if test "z$PLATFORM" == "zLinux_x86"; then
   AC_MSG_RESULT([Linux_x86])
   HAVE_PLATFORM=yes
elif test "z$PLATFORM" == "z"; then
   AC_MSG_RESULT([not specified (defaults to 'Linux_x86')])
   PLATFORM=Linux_x86
   HAVE_PLATFORM=yes
else
   AC_MSG_RESULT([unknown platform])
   HAVE_PLATFORM=no
fi

if test "z$HAVE_PLATFORM" != "zyes"; then
   AC_MSG_ERROR(platform not found.)
fi
AC_SUBST(PLATFORM)

OOO_INSTALL_PATH=$with_ooo
AC_MSG_CHECKING([checking OO.o installation])
if test -f "$OOO_INSTALL_PATH/program/soffice.bin"; then
   HAVE_OOO_INSTALL=yes
   AC_MSG_RESULT([found])
else
   HAVE_OOO_INSTALL=no
   AC_MSG_RESULT([not found])
fi
if test "z$HAVE_OOO_INSTALL" != "zyes"; then
   AC_MSG_ERROR(The OO.o installation is not found.)
fi
AC_SUBST(OOO_INSTALL_PATH)

OOO_SDK_PATH=$with_ooo_sdk
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([checking OO.o SDK])
if test -f "$OOO_SDK_PATH/include/cppuhelper/implementationentry.hxx"; then
   HAVE_OOO_SDK=yes
   AC_MSG_RESULT([found])
else
   HAVE_OOO_SDK=no
   AC_MSG_RESULT([not found])
fi

if test "z$HAVE_OOO_SDK" != "zyes"; then
   AC_MSG_ERROR(The OO.o SDK path is not correct.  Adjust the path and try again.)
fi
AC_SUBST(OOO_SDK_PATH)

BINDIR=$OOO_SDK_PATH/linux/bin
AC_SUBST(BINDIR)

UNO_INCLUDE=-I$OOO_SDK_PATH/include
AC_SUBST(UNO_INCLUDE)

LIBLPSOLVE_PATH=$with_lpsolve
AC_LANG_C
AC_CHECK_HEADER($LIBLPSOLVE_PATH/lp_lib.h, have_lpsolve=true, have_lpsolve=false)
if ! $have_lpsolve; then
   AC_MSG_ERROR(lpsolve path not correct)
fi
AC_SUBST(LIBLPSOLVE_PATH)

LIBLPSOLVE_ARCHIVE_PATH=$LIBLPSOLVE_PATH/lpsolve55/liblpsolve55.a
AC_SUBST(LIBLPSOLVE_ARCHIVE_PATH)

GEN_UNOHEADERS_TARGET="\$(OBJDIR)/unoheaders"
AC_SUBST(GEN_UNOHEADERS_TARGET)

RUNTIME_LIB_PATH=$OOO_INSTALL_PATH/program
AC_SUBST(RUNTIME_LIB_PATH)

AC_OUTPUT(Makefile)

