#!/usr/bin/perl -w

sub revert_touched_files
{
    my $patch = shift;
    my $dest_dir = shift;
    my $file;
    my $orig_file;
    my $Patch;

    $dest_dir && $patch || die "Invalid args";

    open ($Patch, "$patch") || die "Can't open patch $patch";
    while (<$Patch>) {
	if (/^\+\+\+ ([^ \t]*)/) {
	    $file = $1;
	    chomp ($file);
	    $file = $dest_dir.'/'.$file;
#	    print ("File '$file' '$dest_dir/$file.orig'\n");
	    $orig_file = $file.'.orig';
	    if (-f $orig_file) {
		
		my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
		    $atime,$mtime,$ctime,$blksize,$blocks)
		    = stat($orig_file);
		my $vmode = $mode & 0777;
		if ($size == 0 && $vmode == 0) {
		    $quiet || print "Removing new & orig file - originally null $orig_file\n";
		    unlink ($file);
		    unlink ($orig_file);
		} else {
		    $quiet || print "Restoring $orig_file\n";
		    rename ($orig_file, $file) || die "Failed to rename $!";
		}
	    } else {
		$quiet || print "Removing patched file $file\n";
		unlink ($file);
	    }
	}
    }
    close ($Patch);
}

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
foreach $a (@ARGV) {
	if ($a eq '-R') {
    		print ("Removing patched files for update ...\n");
	    	$remove = 1;
	}
	elsif ($a eq '--quiet') {
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

if ($remove) {
    @Patches = reverse @Patches;
}

for $patch_file (@Patches) {
    if ($remove) {
	revert_touched_files ($patch_file, $dest_dir);
    } else {
	$cmd = $base_cmd;
	if ($quiet) {
	    $cmd .= " >& /dev/null ";
	}
	$cmd .= " < $patch_file";
	$quiet || print "$cmd\n";
	system ($cmd) && die "Failed to patch $patch_file: $!";
    }
}
