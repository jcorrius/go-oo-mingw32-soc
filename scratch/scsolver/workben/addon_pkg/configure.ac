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

SCSOLVERDIR=../..
AC_SUBST(SCSOLVERDIR)

OBJDIR=./obj
AC_SUBST(OBJDIR)

OOO_INSTALL_PATH=$(echo $with_ooo | sed -e 's/^~/$(HOME)/g')
AC_SUBST(OOO_INSTALL_PATH)

OOO_SDK_PATH=$(echo $with_ooo_sdk | sed -e 's/^~/$(HOME)/g')
AC_SUBST(OOO_SDK_PATH)

BINDIR=$OOO_SDK_PATH/linux/bin
AC_SUBST(BINDIR)

UNO_INCLUDE=-I$OOO_SDK_PATH/include
AC_SUBST(UNO_INCLUDE)

LIBLPSOLVE_PATH=$(echo $with_lpsolve | sed -e 's/^~/$(HOME)/g')
AC_SUBST(LIBLPSOLVE_PATH)

LIBLPSOLVE_ARCHIVE_PATH=$LIBLPSOLVE_PATH/lpsolve55/liblpsolve55.a
AC_SUBST(LIBLPSOLVE_ARCHIVE_PATH)

GEN_UNOHEADERS_TARGET="\$(OBJDIR)/unoheaders"
AC_SUBST(GEN_UNOHEADERS_TARGET)

RUNTIME_LIB_PATH=$OOO_INSTALL_PATH/program
AC_SUBST(RUNTIME_LIB_PATH)

AC_OUTPUT(Makefile)

