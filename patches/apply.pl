#!/usr/bin/perl -w

use File::Copy;
use File::Basename;

sub get_search_paths()
{
    my @paths = ();
    my @search = split /:/, $options{'PATCHPATH'};

    for my $stem (@search) {
	push @paths, "$patch_dir/$stem";
    }
#    print "Search paths are '@paths'\n";
    return @paths;
}

sub find_patch_file($)
{
    my $file = shift;
    my $file_path = '';

    $file =~ m/\.diff$/ || die "Patch file '$file' is not a .diff";

    for my $path (get_search_paths()) {
	if (-f "$path/$file") {
	    $file_path = "$path/$file";
	    last;
	}
    }

    -f $file_path || die "\n\n** Error ** - Can't find file $file " .
	"in patch path '" . $options{'PATCHPATH'} . "'\n\n\n";

    return $file_path;
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

    $cmd_pipe = " < $patch";
    my $cmd = "( $base_cmd --dry-run ".$cmd_output.$cmd_suffix.")".$cmd_pipe;

    print "$cmd\n" unless $quiet;
    system ($cmd) && die "Testing patch $patch_file failed.";

    $cmd = "( $base_cmd > /dev/null".$cmd_suffix.")".$cmd_pipe;
    if($quiet)
        { print "applying..." }
    else
        { print "$cmd\n" }

    system ($cmd) && die "Failed to patch $patch_file\n" .
			 "- this is often caused by duplicate sections in a patch.\n".
			 "you will need to manually reverse this patch\n";
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

sub select_subset ($$);

sub select_subset ($$)
{
    my $subsets = shift;
    my $tag = shift;
    my @nodes = ();

    if (defined $subsets->{$tag}) {
	@nodes = @{$subsets->{$tag}};
    }
	
    if (!@nodes) {
	return @nodes;
    }

    my $subtag;
    my @result = @nodes;

    my $count = @nodes;
    for $subtag (@nodes) {
	my @subset = select_subset($subsets, $subtag);
	push @result, @subset;
    }

    return @result;
}

sub canonicalize_milestone($)
{
    my $milestone = shift;
    $milestone =~ m/([0-9]+)$/ || return $milestone;
    my $stem = $milestone;
    my $num = $1;
    $num = sprintf("%04d", $num);
    $stem =~ s/([0-9]+)$//;
    $milestone = $stem . $num;
#    print "Canonicalize milestone to '$milestone'\n";
    return $milestone;
}

sub milestone_cmp($$)
{
    my $a = shift;
    my $b = shift;
    $a = canonicalize_milestone($a);
    $b = canonicalize_milestone($b);
    return $a cmp $b;
}

sub rules_pass($)
{
    my $original_rule = shift;
    my $rule = $original_rule;

    while ($rule ne '') {
	my $lastrule = $rule;
#	print "verify rule '$rule'\n";
	# equal to (==)
	if ($rule =~ s/\=\=\s*(\S+)// && milestone_cmp ($tag, $1) != 0 ) { return 0; }; 
	# less than or equal (<=)
	if ($rule =~ s/\<=\s*(\S+)// && milestone_cmp ($tag, $1) > 0 ) { return 0; };
	# less than (<)
	if ($rule =~ s/\<\s*(\S+)// && milestone_cmp ($tag, $1) >= 0 ) { return 0; };
	# greater than or equal (>=)
	if ($rule =~ s/\>=\s*(\S+)// && milestone_cmp ($tag, $1) < 0 ) { return 0; }; 
	# greater than (>)
	if ($rule =~ s/\>\s*(\S+)// && milestone_cmp ($tag, $1) <= 0 ) { return 0; }; 

	$rule =~ s/^\s*//;

	$lastrule eq $rule && die "Invalid syntax in rule: $original_rule\n";
    }
    return 1;
}

sub version_filter_targets($)
{
    my $tlist = shift;
    my @targets;

    $tlist =~ m/([^<>]*)(.*)/;
    my $rules = $2;
    my $targets = $1;
    $targets =~ s/\s*$//;

    @targets = split /\s*,\s*/, $targets;

#    printf "Rules '$rules' targets '$targets'\n";

    if (!rules_pass ($rules)) {
#	printf "Rule '$rules' failed ['@targets']\n";
	@targets = ();
    } else {
#	printf "Rule '$rules' passed ['@targets']\n";
    }

    return @targets;
}

sub patched_files($) {
    my ( $file ) = @_;
    
    my @lines = split (/\n/, $file);
    my @dest;

    foreach $line (@lines) {
        if ( $line =~ /\+\+\+ ([^\s]*)/ ) {
            push @dest, $1;
        }
    }

    return @dest;
}

sub is_dependent_patch($$) {
    my ( $patchedref, $patch ) = @_;
    
    my @files = patched_files ($patch);

    foreach $file (@files) {
        if ( exists $patchedref->{$file} ) {
            return 1;
        }
    }

    return 0;
}

sub list_patches(@) {
    my ( @distros ) = @_;

    my @Patches = ();
    foreach $distro ( @distros ) {
	@Patches = ( @Patches, list_patches_single( $distro ) );
    }

    return @Patches;
}

sub list_patches_single {

    sub checkTerminal {
	my $subsets = shift;
	my $referenced = shift;
	my $distro = shift;

	if (defined $referenced->{$distro} ||
	    !defined $subsets->{$distro}) {
	    print "Error: you must use a terminal node for distro:\n";
	    my $ref;
	    my @terminals;
	    print "    One of: ";
	    for $ref(keys %{$subsets}) {
		if (!defined $referenced->{$ref}) {
		    push @terminals, $ref;
		}
	    }
	    print join ', ', sort (@terminals);
	    print "\n";
	    exit (1);
	}
    }

    my $forDistro = shift;
    my @Patches = ();

    open (PatchList, "$apply_list") || die "Can't find $apply_list";

    my @targets=();
    my %subsets=();
    my @selected_subsets=();
    my %referenced=();
    while (<PatchList>) {
            s/\s*#.*//;
            chomp;
	    s/\r//; # Win32
	    s/\s*$//;
	    s/^\s*//;
            $_ eq '' && next;

	    # continuation
	    if (defined $lastline) {
		$_ = $lastline . $_;
		undef $lastline;
	    }
	    if (m/\\\S*$/) {
		s/\\//g;
		$lastline = $_;
		next;
	    }

            if (/\[\s*(.*)\]/) {
                my $tmp = $1;
                $tmp =~ s/\s+$//;
#               print "Target: '$tmp'\n"; 
		@targets = version_filter_targets($tmp);
                next;
            }

            if (/\s*([_\S]+)\s*=\s*(.*)/) {
		$options{$1} = $2;
		print "$1 => $2\n" unless $quiet;
		next;
	    }

            if (/\s*([_\S]+)\s*:\s*(.*)/) {
		my $key = $1;
		my @value = (split /\s*,\s*/, $2);
		$subsets{$key} = [@value];
		@selected_subsets = select_subset (\%subsets, $forDistro);
#		print "selected sets: @selected_subsets\n" unless $quiet;
		# update terminality map
		my $ref;
		for $ref(@value) {
		    $referenced{$ref} = 1;
		}
                next;
            }

	    push @unfiltered_patch_list, $_;

	    my $set;
	    my $match = 0;
	    for $set (@targets) {
		if (grep /^$set$/i, @selected_subsets) {
		    $match = 1;
		}
	    }
	    if (!$match) {
#		print "@subsets: @targets: skipping '$_'\n";
		next;
	    }

            push @Patches, find_patch_file ($_);
    }
    close (PatchList);

    checkTerminal (\%subsets, \%referenced, $forDistro);

    return @Patches;
}

sub applied_patches_list
{
    my @patches;
    
    foreach (glob($applied_patches."/???-*")) { 
	m/\~$/ && next; # backup file.
	push @patches, $_;
    }

    return @patches;
}

sub apply_patches {

    my @Patches = list_patches (@distros);

    print "\n" unless $quiet;

    for $opt (@required_opts) { 
        defined $options{$opt} || die "Required option $opt not defined"; 
    }

    if( ! -d $applied_patches ) {
        mkdir $applied_patches || die "Can't make directory $patch_dir: $!";
    }

    my %existing_patches;

    foreach (applied_patches_list()) {
        my $file = basename $_;

        $file =~ s/^([0-9]{3})-//;
        $existing_patches{$file} = $_;
    }

    my %obsolete_patches = %existing_patches;
    foreach $patch (@Patches) {
	delete $obsolete_patches{basename($patch)};
    }

    my @to_apply;
    my @to_unapply;
    my %patched;

    foreach $a (keys %obsolete_patches) {
	$patch = $obsolete_patches{$a};

        unshift @to_unapply, $patch;
        foreach $pf ( patched_files (slurp ($patch)) ) {
            $patched{$pf} = 1;
        }

	delete $existing_patches{$a};
    }

    foreach $patch (@Patches) {
        my $patch_file = basename($patch);
        
        my $is_applied = 0;
        
        if( exists $existing_patches{$patch_file} ) {
            my $applied_patch = $existing_patches{$patch_file};
            my $patch_content = slurp($patch);
            my $applied_content = slurp($applied_patch);
            if (length ($patch_content) == length ($applied_content) &&
                $patch_content eq $applied_content &&
                !is_dependent_patch (\%patched, $applied_content))
            {
                print "$patch_file already applied, skipped\n";
                $is_applied = 1;
            }
            else {
                unshift @to_unapply, $applied_patch;
                foreach $pf ( patched_files( $applied_content ) ) {
                    $patched{$pf} = 1;
                }
            }
            delete $existing_patches{$patch_file};
        }

        if (!$is_applied) {
            push @to_apply, $patch;
        }
    }

    foreach $patch ( sort { $b cmp $a } @to_unapply ) {
        print "\n" unless $quiet;
        print "Unapplying patch $patch\n";
        do_patch $patch, $base_cmd." -R";
        unlink $patch || die "Can't remove $patch $!";
    }

    my $patch_num = 0;
    foreach (applied_patches_list()) {
        my $file = basename $_;

        if ( $file =~ /^([0-9]{3})-/ ) {
            my $num = $1;
            $num =~ s/^0*//;
            
            $patch_num = $num if ( $num > $patch_num );
        }
    }
    $patch_num++;

    foreach $patch (@to_apply) {
        my $patch_file = basename($patch);
        print "\n" unless $quiet;
        do_patch $patch, $base_cmd;

        my $patch_copy = sprintf("%s/%03d-%s", $applied_patches, $patch_num++, $patch_file);

        print "copy $patch_file -> $patch_copy\n" unless $quiet;

        copy($patch, $patch_copy) || die "Can't copy $patch to $patch_copy $!";
    }

    if (keys %existing_patches) {
	die "Error - leftover obsolete patches\n";
    }
}

sub remove_patches {
    my @Patches = ();

    -d $applied_patches || return;

    foreach $patch_file (reverse (applied_patches_list())) {
        print "\nRemoving ".basename($patch_file)."...\n" unless $quiet;
        do_patch $patch_file, $base_cmd;
        unlink $patch_file;
    }
    rmdir $applied_patches
}

sub export_series {
    my @Patches = list_patches (@distros);

    for my $patch (@Patches) {
	$patch =~ s/^\Q$patch_dir\E\/..\///;
	print "$patch -p0\n";
    }
}

sub is_old_patch_version()
{
    my $Patch;
    my $ver_line;
    my $is_old = 1;

    open ($Patch, "patch --version|") || die "Can't run patch: $!";
    $ver_line = <$Patch>;
    $ver_line =~ m/\s+(\d+)\.(\d+)\.(\d+)/ || die "Can't get patch version\n";
    if ($1 >= 2 && $2 >= 5 && $3 >= 9) {
	$is_old = 0;
    }
    
    if ($is_old) {
	print "Old patch version - pruning LFs\n";
    }
    return $is_old;
}


sub scan_unused($$)
{
    my $dir = shift;
    my $patches = shift;
    my $dirh;
    my $warned = 0;

    opendir($dirh, $dir) || die "Can't open $dir: $!";
    while (my $file = readdir ($dirh)) {
	$file =~ /^\./ && next;     # hidden
	$file =~ /\.diff$/ || next; # non-patch

	if (!defined $patches->{$file}) {
	    if (!$warned) {
		print "Warning: unused files in $dir\n";
		$warned++;
	    }
	    print "\t$file\n";
	}
    }
    closedir($dirh);
}

sub check_for_unused ($$)
{
    my $patch_dir = shift;
    my $patches = shift;
    my %patches;
    for my $patch (@{$patches}) {
#	print "Check for $patch\n";
	$patches{$patch} = 1;
    }

    for my $path (get_search_paths()) {
	scan_unused ($path, \%patches);
    }
}

(@ARGV > 1) ||
    die "Syntax:\n".
    "apply <path-to-patchdir> <src root> --tag=<src680-m90> [--distro=Debian [--distro=Binfilter [...]]] [--quiet] [--dry-run] [ patch flags ]\n" .
    "apply <path-to-patchdir> --series-to\n" .
    "apply <path-to-patchdir> --find-unused\n";

%options = ();

$quiet = 0;
$remove = 0;
$export = 0;
$opts = "";
@distros = ();
$tag = '';
$dry_run = 0;
$find_unused = 0;
@required_opts = ( 'PATCHPATH' );
@arguments = ();
@unfiltered_patch_list = ();


foreach $a (@ARGV) {
	if ($a eq '-R') {
	    $remove = 1;	    
	    push @arguments, $a;
	} elsif ($a =~ m/--series-from=(.*)/) {
	    $export = 1;
	    $quiet = 1;
	    push @distros, $1;
	} elsif ($a eq '--quiet') {
	    $quiet = 1;
	} elsif ($a =~ m/--distro=(.*)/) {
	    push @distros, $1;
	} elsif ($a =~ m/--find-unused/) {
	    $find_unused = 1;
	} elsif ($a =~ m/--tag=(.*)/) {
	    $tag = $1;
	} elsif ($a =~ m/--dry-run/g) {
	    $dry_run = 1;
	} else {
	    push @arguments, $a;
	}
}

if ( ! @distros ) {
    @distros = ( "Debian" );
}

$patch_dir = shift (@arguments);
substr ($patch_dir, 0, 1) eq '/' || die "apply.pl requires absolute paths";

$apply_list = $patch_dir.'/apply';

print "Execute with $opts for distro(s) '" . join( " ", @distros ) . "'\n" unless $quiet;

if ($dry_run || $find_unused) {
    $tag = '' if ($find_unused);
    my @Patches = list_patches (@distros);
    if ($find_unused) {
	check_for_unused ($patch_dir, \@unfiltered_patch_list);
    } else {
	printf "Dry-run: exiting before applying patches\n";
    }
} elsif ($export) {
    export_series();

} else {
    $dest_dir = shift (@arguments);
    substr ($dest_dir, 0, 1) eq '/' || die "apply.pl requires absolute paths";
    $applied_patches = $dest_dir.'/applied_patches';

    $opts = join ' ', @arguments;
    $base_cmd = "patch -l -p0 $opts -d $dest_dir";
    if (is_old_patch_version()) {
	$base_cmd = 'sed \'s/^\(@.*\)\r$/\1/\' | ' . $base_cmd;
    }

    if ($remove) {
	remove_patches();
    } else {
	apply_patches();
    }
}
