#!/bin/bash

# Find all dependencies based on prj/build.lst (transitive closure)
# [not the ideal implementation, but it was fast to write ;-)]
# The result is in deps.txt

rm -f deps.tmp.[12]

for I in * ; do
    [ -f $I/prj/build.lst ] || continue
    DEPS=`grep -v '^[[:space:]]*#' < $I/prj/build.lst |
          sed -e 's/#.*//' -e 's/[[:space:]]\+/ /g' \
              -e 's/^[^:]*:\+ \?//' -e 's/ \?NULL//' \
              -e 's/\<[^ ]*://g' | head -n1`
    echo $I $DEPS
done > deps.tmp.2

touch deps.tmp.1
while [ "`ls -s --block-size=1 deps.tmp.1 | sed 's/ .*//'`" -ne "`ls -s --block-size=1 deps.tmp.2 | sed 's/ .*//'`" ] ; do
    mv deps.tmp.2 deps.tmp.1
    while read MODULE DEP ; do
        echo -n "$MODULE "
        for I in $DEP ; do
            grep "^\<$I\>" deps.tmp.1 || echo $I
        done | tr ' ' '\n' | sort | uniq | tr '\n' ' '
        echo
    done < deps.tmp.1 > deps.tmp.2
done

sed 's/  \?/: /' < deps.tmp.1 > deps.txt
rm deps.tmp.[12]
