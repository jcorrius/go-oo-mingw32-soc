#!/bin/bash

[ -f prj/build.lst ] || { echo -e "create-tests-lst.sh: Creates prj/tests.lst\nError: Must be in project subdir" ; exit 1 ; }

PREFIX=`grep nmake prj/build.lst | head -n 1 | sed 's/[[:space:]].*//'`
MODULE=`pwd | sed 's#.*/\([^/]*\)$#\1#'`

find . -name makefile.mk -a \
    \( -wholename "*workben*" -o -wholename "*/test/*" \) | \
    while read DIR ; do
        DIR=${DIR#./}
        DIR=${DIR%/makefile.mk}
        SYMBOL="`echo $DIR | sed 's#/#_#g'`"
        DIR="`echo $DIR | sed 's#/#\\\#g'`"
        printf "%-4s %-50s nmake - all ${PREFIX}_$SYMBOL NULL\n" $PREFIX $MODULE\\$DIR
    done
