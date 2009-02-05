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

sub analyze_path($)
{
    my $pf = shift;
    my $topdir = undef;
    my $subpath = undef;
    
#    print "analyzing path: $pf\n";
    $pf =~ s|\.\/+||;
    if ( $pf =~ m/^([^\/]+)([^ \t]+)/ ) {
	$topdir = "$1";
	$subpath = "$2";
	chomp $subpath;
    }
    
    return $topdir, $subpath;
}

my $applying = 0;
my $firstline = undef;
my $firstline_topdir = undef;
my $firstline_subpath = undef;
my $sections = 0;
while (<STDIN>) {
    my $line = $_;

    # unified context patches:
    # 	first line:  --- path
    #	second line: +++ path
    # copied context patches:
    # 	first line:  *** path
    #	second line: --- path
    # IMPORTANT: we need extra reqular expression for --- and *** to do not match lines like -**
    if  ( ( ! defined $firstline ) &&
          ( ( $line =~ m/^\*\*\* [ \t]*([^\s]+)/ ) ||
	    ( $line =~ m/^--- [ \t]*([^\s]+)/ ) ) ) {
	# found first line that defines the path of the patched file
	($firstline_topdir, $firstline_subpath) = analyze_path("$1");
	$firstline = $line;
	next;
    }

    # there are diffs with just the +++ line => the first line is missing in this cases
    if ( ( ( defined $firstline ) && ( $line =~ m/^--- [ \t]*([^\s]+)/ ) ) ||
         ( $line =~ m/^\+\+\+ [ \t]*([^\s]+)/ ) ) {
	# found second line that defines the path of the patched file
	($topdir, $subpath) = analyze_path("$1");

	# need to check all ***, +++, --- paths; any of them might be correct
	# the other path might be invalid, e.g. absolute path
	# the relative paths starting with ".." must be invalid because we use
	# the -p0 patches and the upper direcotry name is different with every milestone
	if ( defined $topdir && "$topdir" ne ".." && -d "$applydir/$topdir" ) {
#	    print "topdir: $topdir\n";
	    $applying = 1;
	} elsif ( defined $firstline_topdir && "$firstline_topdir" ne ".." && -d "$applydir/$firstline_topdir" ) {
#	    print "firstline_topdir: $firstline_topdir\n";
	    $applying = 1;
	    $topdir = $firstline_topdir;
	    $subpath = $firstline_subpath;
	} else {
	    $applying = 0;
	}

	if ($applying) {
	    $sections++;
	    print STDERR "+ apply fragment for $topdir$subpath\n";
	}
    
    }

    if (defined $firstline) {
	# either it is the real first line that defines the file path
	# or it might be the hunk definition in the copied context patches
	# we should print it in both cases
	$line = $firstline . $line;
	$firstline = undef;
    }

    if ($applying) {
	print $fh $line;
    }
}

my $result = 0;
if ($sections > 0) {
# patch complains a lot with empty input
#   print "calling: patch @ARGV < $tmpfile\n";
    if (system ("patch @ARGV < $tmpfile")) {
	 print STDERR "\nError: failed to apply patch @ARGV: $!\n\n";
    }
    $result = $? >> 8;
} else {
    print STDERR "- skipped whole patch\n";
}

unlink $tmpfile;

exit $result;
