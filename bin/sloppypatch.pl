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
my $minusline_dir = '';
my $minusline_subpath = '';
my $sections = 0;
while (<STDIN>) {
    my $line = $_;
    if ( $line =~ m/^--- [ \t]*\.?\/?([^\/]+)([^ \t]+)/ ) {
	$minusline_topdir="$1";
	$minusline_subpath="$2";
	$minusline .= $line;
	next;
    }
    if ( $line =~ m/^\+\+\+ [ \t]*\.?\/?([^\/]+)([^ \t]+)/ ) {
	my $topdir="$1";
	my $subpath="$2";

	# need to use the --- path when the file should get removed
	chomp $subpath;
	if ("$topdir$subpath" eq "dev/null") {
	    $topdir="$minusline_topdir";
	    $subpath="$minusline_subpath";
	}

	$eliding = ! -d "$applydir/$topdir";

	if (!$eliding) {
	    $sections++;
	    print STDERR "+ apply fragment for $topdir$subpath\n";
	}
	$line = $minusline . $line;
	$minusline = '';
	$minusline_topdir='';
	$minusline_subpath='';
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
