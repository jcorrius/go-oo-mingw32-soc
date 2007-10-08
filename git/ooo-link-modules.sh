#! /bin/bash

MODULESDIR="cvsup"
LINKDIR="ooo"

[ -d "$LINKDIR" ] && { echo "$LINKDIR already exists" ; exit 1 ; }

# breaks a dir into a set of symlinks
function linkdir() {
    DIR="$1"
    TMP=""
    for I in `echo "$DIR" | sed 's#/# #g'`; do
        if [ -z "$TMP" ] ; then
            TMP="$I"
        else
            TMP="$TMP/$I"
        fi
        LINK=""
        if [ -h "$TMP" ] ; then
            LINK=`readlink "$TMP"`
            rm "$TMP"
            mkdir "$TMP"
            (
                cd "$TMP"
                for J in ../"$LINK"/* ../"$LINK"/.[^.]* ; do
                    [ -e "$J" ] && { ln -s "$J" . ; }
                done
            )
        fi
    done
}

mkdir -p "$LINKDIR"
mkdir "$LINKDIR/tmp"

cp -a "$MODULESDIR/CVSROOT" "$LINKDIR/"

MODULES="$LINKDIR/CVSROOT/modules.save"
mv "$LINKDIR/CVSROOT/modules" "$MODULES"

ONLY="`grep '^OpenOffice2 -a' "$MODULES" | sed 's/^OpenOffice2 -a //'`"
ONLY=" $ONLY "

# link everything to openoffice.org subdir
echo "Linking modules"
SKIP=9
while read A R ; do
    if [ "$SKIP" -gt "0" ] ; then
        SKIP=$(( $SKIP - 1 ))
        continue
    fi
    [ "$ONLY" != "${ONLY/ $A /XYZ}" ] && ln -s "../../$MODULESDIR/$R" "$LINKDIR/tmp/$A"
done < "$MODULES"

# not in the alias
ln -s "../../xml/oox" "$LINKDIR/tmp/oox"

# not needed
rm -f "$LINKDIR/tmp/rvpapi"

# move the modules according to ooo-link-modules.list
grep -v "^#" ooo-link-modules.list | while read MOD ; do
    D=${MOD%/*}
    F=${MOD#*/}
    [ -d "$LINKDIR/$D" ] || mkdir "$LINKDIR/$D"
    mv "$LINKDIR/tmp/$F" "$LINKDIR/$MOD"
done

# anything remained?
NOT_MOVED=`ls $LINKDIR/tmp/`
[ -z "$NOT_MOVED" ] || echo "Not moved: $NOT_MOVED"

# move the localization stuff modules to l10n subdir
echo "Creating ooo-l10n"
find -L "$LINKDIR" -name "localize.sdf,v" | while read F ; do
    DIR="`dirname $F`"
    TO="$LINKDIR/ooo-l10n/${DIR#*/*/}"

    if [ "$DIR" != "$TO" ] ; then
        mkdir -p "$TO"
        linkdir "$DIR"
        mv "$F" "$TO"/
    fi
done

rmdir ooo/tmp
echo "Done"
