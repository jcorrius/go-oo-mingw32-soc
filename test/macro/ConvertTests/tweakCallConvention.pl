#!/usr/bin/perl  -pi.bak -w
    s/(TestLog_.*?)\s/Call $1\( /;
    s/(TestLog_.*)/$1\)/;
