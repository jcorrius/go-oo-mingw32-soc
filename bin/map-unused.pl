#!/usr/bin/perl

# The worst offenders in bloated sym tables:
@map_source = (
    'svx', 'svt', 'sfx', 'xo',
    'tk', 'vcl', 'fwk', 'svl',
    'sb', 'so', 'tl', 'go', 'frm',
    'basctl', 'dba', 'dbu'
);

$mangle = 'gcc3';

if (@ARGV < 2) {
    print "map-unused.pl: /path/into/solver /path/to/toolsdir\n";
    exit (1);
}

my ($install, $toolsdir) = @ARGV;

# poke around
-d $install || die "Can't find $install: $!";
-d $toolsdir || die "Can't find $toolsdir: $!";
-d "$install/lib" || die "Can't find $install/lib: $!";
-d "$toolsdir/map" || die "Can't find $toolsdir/map: $!";

# Create selected map files
for $a (@map_source) {
    my $src = $install ."/lib/lib".$a."641li.so.unused";

    -f $src || die "Can't find $src: $!";
    
    my $dest = "$toolsdir/map/$a-$mangle.map";

    my $srcfile;
    my $destfile;

    print "Build map $dest\n";
    open ($srcfile, $src) || die "Couldn't open $src: $!";
    open ($destfile, ">$dest") || die "Couldn't open $dest: $!";

    print $destfile "{\n";
#    print $destfile "\tglobal: *;\n";
    print $destfile "\tlocal:\n";
    while (<$srcfile>) {
	chomp;
	/^component_/ && next;
	print $destfile "\t$_;\n";
    }
    print $destfile "};\n";

    close ($srcfile);
    close ($destfile);
}

