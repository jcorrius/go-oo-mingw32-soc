#!/usr/bin/perl -w

use File::Temp qw/ :mktemp  /;

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
($fh,$tmpfile) = mkstemp("/tmp/sloopypatch-XXXXXX");

sub path_exists($)
{
    my $path = shift;
    my $topdir = undef;
    my $subpath = undef;

#    print "Checking path: $path\n";
    
    # skip initial ./ is needed
    $path =~ s|\.\/+||;

    if ( $path =~ m/^([^\/]+)([^ \t]+)/ ) {
	$topdir = "$1";
	$subpath = "$2";
	chomp $subpath;
    }
    
    # the relative paths starting with ".." must be invalid because we use
    # the -p0 patches and the upper direcotry name is different with every milestone
    if ( (defined $topdir && "$topdir" ne ".." && -d "$applydir/$topdir" ) || ( -f "$applydir/$path") ) {
#	print "   passed $topdir,$path\n";
	return $path;
    } else {
#	print "   not passed\n";
	return undef;
    }
}

my $applying = undef;
my $sections = 0;
my $path1 = undef;
my $path2 = undef;
my @header_lines = ();
my $header_unified = 0;
my $header_found = 0;
my $line_matched = 0;
while (<STDIN>) {
    my $line = $_;

    # same of unified context patch header:
    # --- file.old	2009-02-26 20:04:16.000000000 +0100 
    # +++ file		2009-02-26 20:04:41.000000000 +0100 
    # @@ -1,26 +1,28 @@ 

    # sample of copied context patch header:
    # *** file.old 2009-02-26 20:04:16.000000000 +0100 
    # --- file     2009-02-26 20:04:41.000000000 +0100 
    # *************** 
    # *** 1,26 **** 
    
    $line_matched = 0;
    
    if (! @header_lines) {
	if ( $line =~ m/^--- [\s]*([^\s]+)/ ) {
	    $path1 = "$1";
	    $header_unified = 1;
	    $line_matched = 1;
	}
	if ( $line =~ m/^\*\*\* [\s]*([^\s]+)/ ) {
	    $path1 = "$1";
	    $header_unified = 0;
	    $line_matched = 1;
	}	    
	# hack to support unified diffs where the --- line is missing
	if ( $line =~ m/^\+\+\+ [\s]*([^\s]+)/ ) {
	    # only line 2
	    $path2 = "$1";
	    $header_unified = 1;
	    $line_matched = 1;
	    # fake line 1
	    $path1 = $path2;
	    push @header_lines, "";
	}
    } elsif (@header_lines == 1) {
	if ($header_unified) {
	    if ( $line =~ m/^\+\+\+ [\s]*([^\s]+)/ ) {
		$path2 = "$1";
		$line_matched = 1;
	    }
	} else {
	    if ( $line =~ m/^--- [\s]*([^\s]+)/ ) {
		$path2 = "$1";
		$line_matched = 1;
	    }
	}
    } elsif (@header_lines == 2) {
	if ($header_unified) {
	    if ( $line =~ m/^\@\@ -(\d+),(\d+) \s*\+\d+,\d+ \s*\@\@/ ) {
		# new file looks like: @@ -0,0 +1,23 @@ 
		$new_file=1 if  (($1 == 0) && ($2 == 0));
		$line_matched = 1;
		$header_found = 1;
#		print "Unified header found\n";
	    }
	} else {
	    if ( $line =~ m/^\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/ ) {
		$line_matched = 1;
	    }
	}
    } elsif (@header_lines == 3) {
	if (! $header_unified) {
	    if ( $line =~ m/^\*\*\* \s*(\d+)(,\d+)? \s*\*\*\*\*/ ) {
		# new file looks like: *** 0 ****
		$new_file = 1 if ( ( $1 == 0 ) && ( ! defined $2 ) );
		$line_matched = 1;
		$header_found = 1;
#		print "Context header found\n";
	    }
	}
    }
    
    if (@header_lines && ! $line_matched ) {
	# looked like header but it is not header in the end
	if ( $applying ) {
	    # print the saved lines if applying this piece
	    foreach (@header_lines) {
		print $fh $_;
	    }
	}
	@header_lines = ();
	# FIXME: should check the previous lines again???
	#        or at least the last one; might be there extra ---, +++ lines without real hunk?
	#        something like?
	#        --- fileA
	#        +++ fileA
	# 	 --- fileB
	# 	 +++ fileB
	#        @@ -10,6 +10,7 @@
	# well this will ve coverted by the hack for missing --- line, see above;
    }
    
    if ( $header_found ) {
	# accept new path without the "path_exists" check only in topdir
	$applying = undef;
	$applying = $path2 if ( $new_file && ( ! $path2 =~ m/^(\.\/+)?[^\/]+\/[^\/]+/ ) );
	# need to check all ***, +++, --- paths; any of them might be correct
	# the other path might be invalid, e.g. absolute path
	$applying = path_exists( $path2 ) if ( ! $applying );
	$applying = path_exists( $path1 ) if ( ! $applying );
	
	if ( $applying ) {
	    $sections++;
	    print STDERR "+ apply fragment for $applying\n";
	    # print the saved header lines
	    foreach (@header_lines) {
		print $fh $_;
	    }
	}
	@header_lines = ();
	$new_file = 0;
	$header_found = 0;
	$line_matched = 0;
    }	    

    if ($line_matched) {
	# save the header line
	push @header_lines, $line;
	$line_matched = 0;
#	print "Matched header line $#header_lines\n";
    } else {
	# no header line => print it if applying this piece
	if ( $applying ) {
	    print $fh $line;
	}
    }
}
$fh->close;

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
