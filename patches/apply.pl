#!/usr/bin/perl -w

use File::Copy;
use File::Basename;

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

sub do_patch {
    my $patch = shift;
    my $base_cmd = shift;
    my $patch_file = basename($patch);
    my $cmd_output = "";
    my $cmd_suffix = "";

    print "$patch_file: testing..."; 

    if ($quiet) 
        { $cmd_output = " > /dev/null " }

    $cmd_suffix .= " < $patch";
    my $cmd = "$base_cmd --dry-run ".$cmd_output.$cmd_suffix;

    print "$cmd\n" unless $quiet;
    system ($cmd) && die "Testing patch $patch_file failed.";

    $cmd = "$base_cmd > /dev/null".$cmd_suffix;
    if($quiet)
        { print "applying..." }
    else
        { print "$cmd\n" }

    system ($cmd) && die "Failed to patch $patch_file.";
    print "done.\n";
}

sub slurp {
    my $from = shift;
    my $save = $/;
    my $FROM;

    undef $/;
    open ($FROM, "<$from");
    # slurp whole file in one big string
    my $content = <$FROM>;
    close ($FROM);
    $/ = $save;

    return $content;
}

sub apply_patches {

    my @Patches = ();

    open (PatchList, "$apply_list") || die "Can't find $apply_list";

    my @targets=($distro);
    my @subsets=($distro);
    while (<PatchList>) {
            s/\s*#.*//;
            chomp;
            $_ eq '' && next;

            if (/\[\s*(.*)\]/) {
                my $tmp = $1;
                $tmp =~ s/\s+$//;
#               print "Target: '$tmp'\n"; 
                @targets = (split /\s*,\s*/, $tmp);
                next;
            }

            if (/\s*([_\S]+)\s*=\s*(.*)/) {
		my $key = $1;
		my $value = $2;
		if ($key =~ /^_/) {
		    if ($key =~ /^_$distro/) {
			@subsets = (split /\s*,\s*/, $value);
			print "selected sets: @subsets\n";
		    }
		} else {
		    $options{$key} = $value;
		    print "$key => $value\n" unless $quiet;
		}
                next;
            }

	    my $set;
	    my $match = 0;
	    for $set (@targets) {
		if (grep /$set/i, @subsets) {
		    $match = 1;
		}
	    }
	    if (!$match) {
#		print "@subsets: @targets: skipping '$_'\n";
		next;
	    }

            push @Patches, find_file ($patch_dir, $_);
    }
    close (PatchList);
    print "\n" unless $quiet;

    for $opt (@required_opts) { 
        defined $options{$opt} || die "Required option $opt not defined"; 
    }

    if( ! -d $applied_patches ) {
        mkdir $applied_patches || die "Can't make directory $patch_dir: $!";
    }

    my $patch_num = 0;
    my %existing_patches;

    foreach (glob($applied_patches."/???-*")) { 
        my $file = basename $_;

        $file =~ s/^([0-9]{3})-//;
        $existing_patches{$file} = $_;
        $patch_num = $1;
    }
    $patch_num++;

    foreach $patch (@Patches) {
        my $patch_file = basename($patch);
        
        my $is_applied = 0;
        
        if( exists $existing_patches{$patch_file} ) {
            my $applied_patch = $existing_patches{$patch_file};
            my $patch_content = slurp($patch);
            my $applied_content = slurp($applied_patch);
            if (length ($patch_content) == length ($applied_content) &&
                $patch_content eq $applied_content) {
                print "$patch_file already applied, skipped\n";
                $is_applied = 1;
            }
            else {
                print "$patch_file changed, unapplying\n";
                do_patch $applied_patch, $base_cmd." -R";
                unlink $applied_patch || die "Can't remove $applied_patch: $!";
            }
            delete $existing_patches{$patch_file};
        }

        if (!$is_applied) {
            print "\n" unless $quiet;
            do_patch $patch, $base_cmd;

            my $patch_copy = sprintf("%s/%03d-%s", $applied_patches, ++$patch_num, $patch_file);

            print "copy $patch_file -> $patch_copy\n" unless $quiet;

            copy($patch, $patch_copy) 
                || die "Can't copy $patch to $patch_copy $!";
        }
    }

    foreach (values %existing_patches) {
        print "Unapplying obsolete patch $_\n";
        do_patch $_, $base_cmd." -R";
        unlink $_ || die "Can't remove $_ $!";
    }
}

sub remove_patches {
    my @Patches = ();

    -d $applied_patches || return;

    foreach $patch_file (reverse glob($applied_patches."/???-*")) {
        print "\nRemoving ".basename($patch_file)."...\n" unless $quiet;
        do_patch $patch_file, $base_cmd;
        unlink $patch_file;
    }
    rmdir $applied_patches
}

(@ARGV > 1) || die "Syntax:\napply <path-to-patchdir> <src root> [--distro=Debian] [patch flags '--dry-run' eg.]\n";

$patch_dir = shift (@ARGV);
$apply_list = $patch_dir.'/apply';
$dest_dir = shift (@ARGV);
$applied_patches = $dest_dir.'/applied_patches';
%options = ();

$quiet = 0;
$remove = 0;
$opts = "";
$distro = 'Ximian';
@required_opts = ( 'PATCHPATH' );

foreach $a (@ARGV) {
	if ($a eq '-R') {
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

# Compatibility for a little ...
if (!($patch_dir =~ /RC3_030729/)) {
    $base_cmd .= ' --fuzz=1';
}

print "Execute: $base_cmd for distro '$distro'\n" unless $quiet;

if (!$remove) {
    apply_patches();
}
else {
    remove_patches();
}

