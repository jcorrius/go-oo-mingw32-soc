#!/usr/bin/perl -w

$total_defined = 0;
%libs = ();
%symbols = ();
%ex_syms = ();
@prefix = ();

$path='/opt/OpenOffice/OOO_STABLE_1/solver/641/unxlngi4.pro';

# for various tools.
$ENV{LD_LIBRARY_PATH}="$path/lib:$ENV{LD_LIBRARY_PATH}";

sub build_libs {
    my $file;
    my $dirhandle;
    my $path = shift;

    opendir ($dirhandle, $path) || die "Can't open dir $path: $!";
    while ($file = readdir ($dirhandle)) {
	if ($file =~ m/\S+.so/ &&
	    !($file =~ m/used/)) {
	    $libs{"$path/$file"} = undef;
	}
    }
    closedir ($dirhandle);
}

sub find_deps {
    my $lib = shift;
    my @deps = ();
    my $ldd;

    open $ldd, "ldd $lib|" || die "Can't ldd $lib: $!";
    
    while (<$ldd>) {
#	print "Dep $_\n";
	/not found/ && die "Missing dep-lib ('$lib'): '$_'\n";
	if (/\s+\S+\s\=\>\s+(\S+)\s+\(/) {
	    if (exists ($libs{$1})) {
		push @deps, $1;
	    }
	}
    }
    close $ldd;

    return @deps;
}

sub insert_symbols {
    my $obj;
    my $defined = 0;
    my $referenced = 0;
    my $lib = shift;
#    my @fixups = ();
    my @external = ();
    my $pref = join ('', @prefix) . ' ';

#    print "$pref inserting from $lib\n";

# Use '-C' to get de-mangled symbols ...
    open $obj, "objdump -T $lib|" || die "Can't objdump $lib: $!";

    while (<$obj>) {
	/[0-9a-f]*\s+([g ])\s+..\s+(\S*).....................\s+(.*)/ || next;
	
	my ($symbol, $type) = ($3, $2);

	if (!$symbol || !$type) {
	    next;
	}

	if ($type eq '.text') {
	    if (exists $symbols{$symbol}) {
# FIXME: investigate all these
#		print "$pref possible duplicate def of '$symbol'\n";
	    } else {
#		print "$pref define symbol: '$symbol'\n";
		$symbols{$symbol} = $lib;
	    }
	    $defined++;
	    $total_defined++;
	} elsif ($type eq '*UND*') {
	    push (@external, $symbol);
	}
    }

    for $symbol (@external) {
	if (exists $symbols{$symbol}) {
	    my $val;
	    $referenced++;
	    $val = $symbols{$symbol};
	    $val =~ m/^r-/ || ($symbols{$symbol} = "r-$val");
	} else {
	    $ex_syms{$symbol} = "$lib";
	}
    }

    print "$pref $defined defined, $referenced referenced\n";
    
    close $obj;
}

sub resolve_symbols {
    my $lib = shift;
    my @deps;

    if ($libs{$lib}) {
	return;
    }

    @deps = find_deps ($lib);

    print join ('', @prefix), "resolving library $lib:\n";
    push @prefix, ' ';

    for $resolve (@deps) {
	resolve_symbols ($resolve);
    }

    insert_symbols ($lib);

    pop @prefix;

    $libs{$lib} = 'resolved';
}

# Main:

my $instset = 0;

if ($instset) {
    # This path misses the symbols
    # needed by the builtin utils
    build_libs ($path);
    build_libs ("$path/filter");
    build_libs ("$path/components");
    # custom nastiness
    $libs{"$path/soffice.bin"} = undef;
    $libs{"$path/setup.bin"} = undef;
} else {
    build_libs ("$path/lib");
}

# builtin tools ...
@tools = ('autodoc', 'bmp', 'bmpgui', 'cfgex',
	  'checkdll', 'checkscp', 'checksingleton',
	  'cppumaker', 'g2g', 'idlc',
	  'localize', 'lzip', 'makedepend',
	  'proxy', 'rdbmaker', 'regcomp', 'regmerge',
	  'regview', 'regcompare', 'rsc', 'rsc2',
	  'rscdep', 'rscpp', 'scpcomp', 'scplink',
	  'scppkg', 'scpzip', 'srvdepy', 'svidl',
	  'transex3', 'uno', 'xml2cmp', 'xrmex',
	  'setup.bin', 'loader.bin', 'soffice.bin',
	  'ucb.bin' );

for $bin (@tools) {
    $libs{"$path/bin/$bin"} = undef;
}

for $lib (keys %libs) {
    resolve_symbols ($lib);
}

my %unused = ();

printf "Finding unused...";
for $lib (keys %libs) {
    my $used = 0;
    my $unused = 0;
    my $used_file;
    my $un_used_file;
    
    open ($used_file, ">$lib.used") || die "Failed to open $lib.used: $!";
    open ($un_used_file, ">$lib.unused") || die "Failed to open $lib.unused: $!";

    for $symbol (keys (%symbols)) {
	my $sym_lib = $symbols {$symbol};
	
	if ($sym_lib eq $lib) {
	    print $un_used_file "$symbol\n";
	    $unused++;
	} elsif ($sym_lib eq "r-$lib") {
	    print $used_file "$symbol\n";
	    $used++;
	}
    }

    close ($used_file);
    close ($un_used_file);

    print "Library $lib: $used used, and $unused unused\n";
}

my $ex_file;
open ($ex_file, ">external.used") || die "Failed to open external.used: $!";
for $symbol (keys %ex_syms) {
    my $hmm = '';
    if ($symbols{$symbol}) {
	$hmm = 'defined later (!)';
    }
    print $ex_file "$symbol; $hmm\n";
}
close ($ex_file);
