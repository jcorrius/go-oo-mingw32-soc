#!/usr/bin/perl -w

# find-duplicates:
# Gets the duplicates across modules for the class constructor symbols
# Run this from directory where the *.so are located
# /opt/OOInstall/program or 
# ooo-build/build/src680-mxx/solver/680/unxlngi4.pro/lib
# 

@libs = ();
%symbols = ();

sub insert_symbols {
    my $obj;
    my $referenced = 0;
    my $lib = shift;
    my @external = ();

    open $obj, "objdump -T $lib|" || die "Can't objdump $lib: $!";

    while (<$obj>) {
	/[0-9a-f]*\s+([g ])\s+..\s+(\S*).....................\s+(.*)/ || next;
	
	my ($linkage, $type, $symbol) = ($1, $2, $3);

	$symbol && $type || next;

	# Is this correct ?
	$linkage =~ /g/ || next; # bin weak symbols.

	if ($symbol =~ /\w+C+[0-9]{1}/){
	 if ($type eq '.text') {
	    $symbol =~ /_GLOBAL_/ && next; # bin global symbols - we don't grok them well

	    my $name = `c++filt $symbol`;
	    my @symb_arr = split /\(/, $name;
	    $name = $symb_arr[0];
	    $name =~ s/\:\:[^\:]*$//;

#	    print "Sane constructor '$name' ($linkage, $type)\n";

	    if (exists $symbols{$name}) {
		if($symbols{$name} ne $lib){
			print "$lib and $symbols{$name} defines '$name' ]\n";
		}
	    } else {
		$symbols{$name} = $lib;
	    }
	 } elsif ($type eq '*UND*') {
	    push (@external, $symbol);
	 }
	}
    }

    close $obj;
}

sub resolve_symbols {
    my $lib = shift;
    if ($libs{$lib}) {
	return;
    }
    insert_symbols ($lib);
    $libs{$lib} = 'resolved';
}

my $file;
my $dirhandle;
my $path = ".";

my @exceptions = ( 'cppuhelper3gcc3', 'uno_cppuhelpergcc',
		   'salhelper3gcc3', 'uno_salhelpergcc' );
sub is_exception($)
{
    my $file = shift;
    for my $exc (@exceptions) {
	if ($file =~ /$exc/) {
	    return 1;
	}
    }
    return 0;
}

opendir ($dirhandle, $path) || die "Can't open dir $path: $!";
while ($file = readdir ($dirhandle)) {
    $file =~ /^\./ && next;
    is_exception($file) && next;

    if ($file =~ m/.so$/) {
	push @libs, "$path/$file";
    }
}
closedir ($dirhandle);

for $lib (@libs) {
    resolve_symbols ($lib);
}
