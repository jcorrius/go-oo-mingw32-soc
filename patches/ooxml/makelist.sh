#!/bin/sh

ls -l | grep -E "\.diff$" | sed \
    -e "s/.*\ oox/oox/g" \
    -e "s/.*\ xmlfilter/xmlfilter/g" \
    -e "s/.*\ writerfilter/writerfilter/g" \
    -e "s/.*\ ooxml/ooxml/g" \
    | sort > list.txt
