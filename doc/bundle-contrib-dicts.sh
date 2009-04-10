NOTE:

NOT TO BE USED ANY MORE. Now we use prebuilt dictionaries, zipped up
now and then on a SLED machine with all dictionaries installed. See
ChangeLog and references to writingaids* in ../download.in,
../bin/unpack and ../patches/src680/win32-prebuilt-writingaids-zip.diff.


#!/bin/sh

# Very hacky script to unpack contributed dictionary packs into the
# build source tree, create dictionary.lst and makefile.mk files for
# them, and update dictionaries/prj/build.lst. This is for Win32 where
# we want to bundle selected contributed dictionaries into the base
# installer, as there is no system myspell or package management
# system.

# This script is more for documentation than to be used as such,
# although it did work for me.

# To use, you should download a selection of the *-pack.zip files from
# http://ftp.stardiv.de/pub/OpenOffice.org/contrib/dictionaries/ or
# one of its mirrors into the src directory. Then run this script,
# then build. (If you already have a build and just want an
# incremental rebuild to add the new dictionaries to the installer,
# run "build" and "deliver" in dictionaries, then "build" in
# instsetoo_native.)

# Note that this script is not clever enough to notice that
# pt_PT-pack.zip contains a dictionary also for pt_BR. Ditto for es_ES
# and es_MX. Another quirk is no_NO-pack.zip which contains
# dictionaries for nb_NO and nn_NO, but not no_NO. Handle those
# manually: Copy pt_PT-pack.zip to pt_BR-pack.zip, and copy
# es_ES-pack.zip to es_MX-pack.zip before running this script. Also
# rename no_NO-pack.zip to nb_NO-pack.zip and copy it to
# nn_NO-pack.zip.

# An even more difficult case is Russian. ru_RU-pack.zip contains
# ru_RU_ye.zip and ru_RU_yo.zip. ru_RU_ye.zip then inconsistently
# contains a dictionary for ru_RU_ie (and not ru_RU_ye). You can spell
# Russian in two "styles" or "variants", "ye" and "yo". But in OOo's
# dictionary.lst you can have just one DICT for "ru RU" anyway, so you
# will have to decide which one to have spell checking for and bundle
# that. If I understand correctly, the "ye" variant is more informal,
# and the "yo" variant more formal, and is the one usually taught to
# non-native speakers and in primary school.

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
