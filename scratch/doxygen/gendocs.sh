#!/bin/bash
#
# Doxygen Doc generation script
#
# Last changes: 06/06/2008
#
###################################################
#
# Config
#
###################################################

# otherwise, aliases are not expanded below
shopt -s expand_aliases

# Title of the documentation
DOXYGEN_PROJECT_PREFIX="OOo"

# branch to draw from
MASTER_BRANCH=DEV300

# scrape latest milestone from EIS
LATEST_MWS=`wget -O - http://eis.services.openoffice.org/EIS2/cws.rss.CWSAnnounceNewsFeed/mws 2>/dev/null | grep "<title>${MASTER_BRANCH}_[a-z0-9]\+ ready for use" | head -n1 | sed -e ' s/.*title>\([A-Z]\+[0-9]\+_[a-z0-9]\+\).*/\1/'`

# fiddle milestone
MILESTONE=`echo $LATEST_MWS | sed -e ' s/[A-Z]\+[0-9]\+_\([a-z0-9]\+\)/\1/'`

# checkout everything referenced from smoketest
echo "Checking out $MASTER_BRANCH $MILESTONE ..."
ooco -v Master=$MASTER_BRANCH -v Milestone=$MILESTONE

# configure some random build -
pushd config_office ; ./configure --disable-odk --disable-mozilla ; popd

# we just need a few paths to solver headers
source Linux*.sh
./bootstrap

# ...and those headers delivered
for DIR in *; do if [ -d $DIR ]; then pushd $DIR; deliver; popd; fi; done

# get list of modules in build order
#INPUT_PROJECTS=`build --all --show | sed -n -e '/Building module/ s/Building module // p'`
INPUT_PROJECTS="basegfx vcl canvas cppcanvas oox svtools goodies xmloff slideshow sfx2 svx chart2 dbaccess sd sc sw"

# output directory for generated documentation
BASE_OUTPUT="/tmp/docs"

# paths for binary and configuration file
BASE_PATH=`pwd`
DOXYGEN_CFG="$HOME/bin/doxygen.cfg"

# strip -I. and bin -I prefix. exlude system headers
DOXYGEN_INCLUDE_PATH=`echo $SOLARINC | sed -e ' s/-I\.//'g | sed -e ' s/ -I/ /'g | sed -e ' s|/usr/[^ ]*| |g'`

# setup version string
DOXYGEN_VERSION="$MASTER_BRANCH $MILESTONE"


###################################################
#
# Generate docs
#
###################################################

# cleanup
rm -rf $BASE_OUTPUT/*

# make the stuff world-readable
umask 022

# generate docs
DOXYGEN_REF_TAGFILES=""
for PROJECT in $INPUT_PROJECTS; 
do
  # avoid processing of full project subdirs, only add source and inc
  DOXYGEN_INPUT=`printf "%s" "$PROJECT/source $PROJECT/inc "`

  DOXYGEN_OUTPUT="$BASE_OUTPUT/$PROJECT"  
  DOXYGEN_OUR_TAGFILE="$DOXYGEN_OUTPUT/$PROJECT.tags"
  DOXYGEN_PROJECTNAME="$DOXYGEN_PROJECT_PREFIX Module $PROJECT"

  # export variables referenced in doxygen config file
  export DOXYGEN_INPUT
  export DOXYGEN_OUTPUT
  export DOXYGEN_INCLUDE_PATH
  export DOXYGEN_VERSION
  export DOXYGEN_OUR_TAGFILE
  export DOXYGEN_REF_TAGFILES
  export DOXYGEN_PROJECTNAME

  # debug
  echo "Calling $DOXYGEN_PATH/doxygen $DOXYGEN_CFG with"
  echo "Input:      $DOXYGEN_INPUT"
  echo "Output:     $DOXYGEN_OUTPUT"
  echo "Include:    $DOXYGEN_INCLUDE_PATH"
  echo "Version:    $DOXYGEN_VERSION"
  echo "Tagfile:    $DOXYGEN_OUR_TAGFILE"
  echo "Ref-Tags:   $DOXYGEN_REF_TAGFILES"
  echo "Title:      $DOXYGEN_PROJECTNAME"
 
  nice -15 doxygen "$DOXYGEN_CFG"

  # setup referenced tagfiles for next round
  DOXYGEN_REF_TAGFILES="$DOXYGEN_REF_TAGFILES $DOXYGEN_OUR_TAGFILE=$BASE_URL/$PROJECT/html"
done

# generate entry page
cat - > $BASE_OUTPUT/index.html <<EOF
<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
    <head>
        <title>OOo Source Code Documentation (fragmentary)</title>
    </head>
    <body>
        <h1>OOo Source Code Documentation (fragmentary)</h1>
        <ul>
EOF

for PROJECT in $INPUT_PROJECTS;
do
  echo "<li><a href=\"$PROJECT/html/index.html\">$PROJECT</a></li>" >> $BASE_OUTPUT/index.html
done

cat - >> $BASE_OUTPUT/index.html <<EOF
        </ul>
        <p>Last updated: 
EOF
date >> $BASE_OUTPUT/index.html

cat - >> $BASE_OUTPUT/index.html <<EOF
        </p>
    </body>
</html>
EOF

## done

