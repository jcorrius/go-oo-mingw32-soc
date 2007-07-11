#!/bin/sh

ls -l | grep -E "\.diff$" | sed -e "s/.*\ oox/oox/g" -e "s/.*\ xmlfilter/xmlfilter/g" > list.txt
