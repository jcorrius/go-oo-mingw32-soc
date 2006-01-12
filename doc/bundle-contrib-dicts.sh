#!/bin/sh

# Very hacky script to unpack contributed dictionary packs (which have
# to have been downloaded into $SRCDIR) into the build source tree and
# update the corresponding prj/build.lst. This is for Win32 where we
# want to bundle selected contrib dictionaries into the base
# installer, as there is no system myspell that some distro package
# management system would handle.

. bin/setup

echo "Unpacking contributed dictionaries into the tree"
for F in $SRCDIR/??*_??-pack.zip; do
    L=`basename $F .zip`
    L=${L%-pack}
    LLANG=${L%_??}
    LLANGU=`echo $LLANG | tr a-z A-Z`
    LCOUNTRY=${L#*_}
    LDIR=$OOBUILDDIR/dictionaries/$L
    if test ! -d $LDIR; then
	T=`mktemp -d`
	unzip $F -d $T
	mkdir $LDIR
	cat >$LDIR/makefile.mk <<EOF
PRJ = ..

PRJNAME	= dictionaries
TARGET	= dict_$L

#----- Settings ---------------------------------------------------------

.INCLUDE : settings.mk

# --- Files --------------------------------------------------------

.IF "\$(DIC_ALL)\$(DIC_${LLANGU}${LCOUNTRY})"!=""

DIC2BIN= \\
EOF
	(
	cd $T

	if test -s spell.txt -a -f $L.zip; then
	    unzip $L.zip
	    if test -f $L.aff -a -f $L.dic; then
		cp $L.aff $L.dic $LDIR
		echo DICT ${L%_??} ${L#??_} $L >>$LDIR/dictionary.lst
		cat >>$LDIR/makefile.mk <<EOF
	$L.aff \\
	$L.dic \\
EOF
	    fi
	fi

	if test -s hyph.txt -a -f hyph_$L.zip; then
	    unzip hyph_$L.zip
	    if test -f hyph_$L.dic; then
		cp hyph_$L.dic $LDIR
		echo HYPH $LLANG $LCOUNTRY hyph_$L >>$LDIR/dictionary.lst
		cat >>$LDIR/makefile.mk <<EOF
	hyph_$L.dic \\
EOF
	    fi
	fi

	if test -s thes.txt -a -f thes_$L.zip; then
	    unzip thes_$L.zip
	    if test -f th_$L.dat -a -f th_$L.idx; then
		cp th_$L.dat th_$L.idx $LDIR
		echo THES $LLANG $LCOUNTRY th_$L >>$LDIR/dictionary.lst
		cat >>$LDIR/makefile.mk <<EOF
	th_$L.dat \\
	th_$L.idx \\
EOF
	    fi
	fi

	for README in README_*.txt; do
	    if test -s $README; then
		cat >>$LDIR/makefile.mk <<EOF
	$README \\
EOF
		cp $README $LDIR
	    fi
	done

	echo >>$LDIR/makefile.mk
	)

	rm -rf $T

	cat >>$LDIR/makefile.mk <<EOF
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : target.mk
.INCLUDE : \$(PRJ)\$/util\$/target.pmk

EOF
    fi
done

# rewrite the prj/build.lst
cat >$OOBUILDDIR/dictionaries/prj/build.lst <<EOF
di      dictionaries :       lingucomponent NULL
di	dictionaries					usr1 - all di_mkout NULL
di	dictionaries\\diclst	nmake	-	all	di_diclst NULL
EOF
LLIST=''
for D in $OOBUILDDIR/dictionaries/*_??; do
    L=`basename $D`
    cat >>$OOBUILDDIR/dictionaries/prj/build.lst <<EOF
di	dictionaries\\$L	nmake	-	all	di_$L di_diclst NULL
EOF
    LLIST="$LLIST di_$L"
done
cat >>$OOBUILDDIR/dictionaries/prj/build.lst <<EOF
di      dictionaries\\util       nmake   -       all     di_util${LLIST} di_diclst NULL
EOF
