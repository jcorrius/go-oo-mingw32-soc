#!/bin/bash

# create .gitignores for the the OOo tree
# run in build/xyz680-mABC

[ -d .git ] || git init

chmod a+x solenv/bin/build.pl
chmod a+x solenv/bin/gccinstlib.pl

# Check for a present git config
if [ ! -w .git/config  ] ; then
    echo ".git/config should be writable"
    exit 1
fi

# Changing the .git/config
cat >> ".git/config" << EOF

[diff "swallow"]
    command = /bin/true
EOF

# Creating the .gitattributes
cat >> ".gitattributes" << EOF
/applied_patches/* diff=swallow
EOF

# Creating the .gitignore
while read F ; do
    D=`dirname "$F"`
    B=`basename "$F"`
    if [ "${F:0:1}" = "/" ] ; then
        echo "$F" >> ".gitignore"
    else
        [ -d "$D" ] && echo "$B" >> "$D/.gitignore"
    fi
done << EOF
/Linux*Env.Set*
/MacOS*Env.Set*
/bootstrap
/makefile.mk
/ID
unxlng*.pro
unxlngi6
unxmacx*.pro
unxmacxi
localize.sdf
.svn
/solver
*.orig
*~
autom4te.cache/
config.log
config.parms
config.status
configure
set_soenv
warn
default_images/introabout/intro-save.bmp
default_images/svx/res/openabout_ark.png
default_images/svx/res/openabout_translateorgza.png
default_images/sw/res/go-oo-team.png
dmake/.deps/
dmake/Makefile
dmake/config.h
dmake/config.log
dmake/config.status
dmake/dag.o
dmake/dbug/dbug/.deps/
dmake/dmake
dmake/dmake.o
dmake/dmakeroot.h
dmake/dmdump.o
dmake/dmstring.o
dmake/expand.o
dmake/function.o
dmake/getinp.o
dmake/hash.o
dmake/imacs.o
dmake/infer.o
dmake/macparse.o
dmake/make.o
dmake/msdos/.deps/
dmake/parse.o
dmake/path.o
dmake/percent.o
dmake/quit.o
dmake/rulparse.o
dmake/stamp-h1
dmake/startup/Makefile
dmake/startup/config.mk
dmake/startup/unix/Makefile
dmake/startup/unix/cygwin/Makefile
dmake/startup/unix/linux/Makefile
dmake/startup/unix/macosx/Makefile
dmake/startup/unix/solaris/Makefile
dmake/startup/unix/sysvr4/Makefile
dmake/startup/winnt/Makefile
dmake/startup/winnt/mingw/Makefile
dmake/startup/winnt/msvc6/Makefile
dmake/stat.o
dmake/state.o
dmake/sysintf.o
dmake/tests/Makefile
dmake/unix/.deps/
dmake/unix/.dirstamp
dmake/unix/arlib.o
dmake/unix/dcache.o
dmake/unix/dirbrk.o
dmake/unix/rmprq.o
dmake/unix/ruletab.o
dmake/unix/runargv.o
dmake/unix/tempnam.o
dmake/win95/.deps/
dmake/win95/microsft/.deps/
instsetoo_native/res/banner_nld.bmp
instsetoo_native/res/nologoinstall_nld.bmp
instsetoo_native/util/OpenOffice
setup_native/source/win32/nsis/ooobanner_nld.bmp
setup_native/source/win32/nsis/ooobitmap_nld.bmp
setup_native/source/win32/nsis/ooosetup_nld.ico
solenv/unxlngx6/
svx/res/
EOF
