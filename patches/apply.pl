#!/usr/bin/perl -w

sub find_file($$)
{
    my $dir = shift;
    my $file = shift;

    if (!-f "$dir/$file") {
	my @search = split /:/, $options{'PATCHPATH'};

	for $stem (@search) {
	    my $testdir = "$dir/$stem";
	    if (-f "$testdir/$file") {
		$dir = $testdir;
		last;
	    }
	}
    }

    -f "$dir/$file" || die "\n\n** Error ** - Can't find file $dir/$file\n\n\n";

    return "$dir/$file";
}

(@ARGV > 1) || die "Syntax:\napply <path-to-patchdir> <src root> [--distro=Debian] [patch flags '--dry-run' eg.]\n";

$patch_dir = shift (@ARGV);
$apply_list = $patch_dir.'/apply';
$dest_dir = shift (@ARGV);
%options = ();

$quiet = 0;
$remove = 0;
$opts = "";
$distro = 'Ximian';
@required_opts = ( 'PATCHPATH' );

foreach $a (@ARGV) {
	if ($a eq '-R') {
    		print ("Reversing patched files ...\n");
	    	$remove = 1;
	}

	if ($a eq '--quiet') {
	    $quiet = 1;
	} elsif ($a =~ m/--distro=(.*)/) {
	    $distro = $1;
	} else {
		$opts = $opts . " " . $a;
	}
}

$base_cmd = "patch -l -b -p0 $opts -d $dest_dir";

if (!$remove) {
    print "Execute: $base_cmd for distro '$distro'\n";
}

@Patches = ();

open (PatchList, "$apply_list") || die "Can't find $apply_list";

my @targets=($distro);
while (<PatchList>) {
	s/\s*#.*//;
	chomp;
	$_ eq '' && next;

	if (/\[\s*(.*)\]/) {
	    my $tmp = $1;
	    $tmp =~ s/\s+$//;
	    print "Distro: '$tmp'\n";

	    @targets = split /\s*,\s*/, $tmp;
	    next;
	}

	if (/\s*(\S+)\s*\s*=(\S+)/) {
	    $options{$1} = $2;
	    print "$1 => $2\n";
	    next;
	}

	if (!grep /$distro/i, @targets) {
	    print "$distro: skipping '$_'\n";
	    next;
	}

	push @Patches, find_file ($patch_dir, $_);
}
close (PatchList);

for $opt (@required_opts) { 
    defined $options{$opt} || die "Required option $opt not defined"; 
}


if ($remove) {
    @Patches = reverse @Patches;
}

print "testing patches...\n";

for $test_patch_file (@Patches) {
	$cmd = $base_cmd." --dry-run";
	if ($quiet) {
	    $cmd .= " > /dev/null ";
	}
	$cmd .= " < $test_patch_file";
	$quiet || print "$cmd\n";
	system ($cmd) && die "Testing patch $test_patch_file failed: $!";
}

print "applying patches...\n";

for $patch_file (@Patches) {
	$cmd = "$base_cmd > /dev/null < $patch_file";
	$quiet || print "$cmd\n";
	system ($cmd) && die "Failed to patch $patch_file: $!";
}
