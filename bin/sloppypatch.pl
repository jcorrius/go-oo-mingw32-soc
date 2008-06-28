#!/usr/bin/perl -w

use POSIX;

# apply a patch, but only if the top-level directory exists
# mentioned in the patch; eg.
#   --- sw/source/ui/foo.xml
#   will apply if sw/ exists.
# Bugs:
#   doesn't cope with '-p' - assumes -p0

my $eliding = 0;
my $minusline = '';
while (<STDIN>) {
    my $line = $_;
    if ( $line =~ m/^--- /) {
	$minusline .= $line;
	next;
    }
    if ( $line =~ m/^\+\+\+ [ \t]*([^\/]+)([^ \t]+)/ ) {
	$eliding = ! -d $1;
	print STDERR (($eliding ? "- skip" : "+ apply") . " fragment for $1$2\n");

	$line = $minusline . $line;
	$minusline = '';
    }
    if (!$eliding) {
	print $line;
    }
}
