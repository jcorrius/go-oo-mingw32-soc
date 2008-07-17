#!/usr/bin/perl -w

use IO::File;
use POSIX qw(tmpnam);

# apply a patch, but only if the top-level directory exists
# mentioned in the patch; eg.
#   --- sw/source/ui/foo.xml
#   will apply if sw/ exists.
# Bugs:
#   doesn't cope with '-p' - assumes -p0

my $applydir;
chomp ($applydir = `pwd`);
for (my $idx = 0; $idx < @ARGV; $idx++) {
    if ($ARGV[$idx] eq '-d') {
	$applydir = $ARGV[$idx+1];
    }
}

my $tmpfile;
my $fh;
for (;;) {
    $tmpfile = tmpnam();
    sysopen( $fh, $tmpfile, O_RDWR | O_CREAT | O_EXCL ) && last;
}

my $eliding = 1;
my $minusline = '';
my $sections = 0;
while (<STDIN>) {
    my $line = $_;
    if ( $line =~ m/^--- /) {
	$minusline .= $line;
	next;
    }
    if ( $line =~ m/^\+\+\+ [ \t]*\.?\/?([^\/]+)([^ \t]+)/ ) {
	$eliding = ! -d "$applydir/$1";

	if (!$eliding) {
	    $sections++;
	    print STDERR "+ apply fragment for $1$2\n";
	}
	$line = $minusline . $line;
	$minusline = '';
    }
    if (!$eliding) {
	print $fh $line;
    }
}

my $result = 0;
if ($sections > 0) {
# patch complains a lot with empty input
    if (system ("patch @ARGV < $tmpfile")) {
	 print STDERR "\nError: failed to apply patch @ARGV: $!\n\n";
    }
    $result = $? >> 8;
} else {
    print STDERR "- skipped whole patch\n";
}

unlink $tmpfile;

exit $result;
