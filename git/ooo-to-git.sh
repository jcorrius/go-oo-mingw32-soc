#! /bin/bash

# CVS_PATH must be absolute
CVS_PATH="/local/git/ooo"
GIT_PATH="ooo-git"

[ -e "$GIT_PATH" ] && { echo "'$GIT_PATH' already exists." ; exit 1 ; }
mkdir -p "$GIT_PATH"

# cvsps cache seems to break things :-(
rm ~/.cvsps/*

COUNT=0
echo -n "Creating "
for I in `( cd "$CVS_PATH" ; echo * )` ; do
    [ "$I" = "CVSROOT" ] && continue

    if [ "$COUNT" -lt "3" ] ; then
	COUNT=$(( $COUNT + 1 ))
    else
	echo "..."
	wait
	COUNT=0
	echo -n "Creating "
    fi

    echo -n "$I "
    git-cvsimport-ooo -v -C "$GIT_PATH/$I.git" -i -k -o master \
                      -m -M 'INTEGRATION: CWS (\w+)' \
                      -d "$CVS_PATH" \
                      "$I" \
                      > "$GIT_PATH/$I.log" 2>&1 &
#if we already have the cvsps output, use  -P "/local/git/ooo-git.cvsps" \
done

echo "..."
wait
echo "Done"
