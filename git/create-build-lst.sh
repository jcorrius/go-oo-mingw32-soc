#!/bin/bash

# creates build.lst for split modules with correct BUILD_TYPE

mkdir -p build/prj

(
echo -n "bu build : "
for I in `echo *` ; do
    [ -d "$I" ] || continue
    [ "$I" = "build" ] && continue

    BTYPE=`head -n1 -q ../*/*/prj/build.lst | grep ":$I\>" | head -n1 | sed "s/^.*\<\([a-zA-Z_0-9]*\):$I\>.*$/\1/"`
    if [ -z "$BTYPE" ] ; then
        echo -n "$I "
    else
        echo -n "$BTYPE:$I "
    fi
done
echo NULL
) > build/prj/build.lst
