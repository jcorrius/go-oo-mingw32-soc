#!/usr/bin/perl -w

# find-duplicates:
# Gets the duplicates across modules for the class constructor symbols
# Run this from directory where the *.so are located
# /opt/OOInstall/program or 
# ooo-build/build/src680-mxx/solver/680/unxlngi4.pro/lib
# 

%libs = ();
%symbols = ();

sub insert_symbols {
    my $obj;
    my $referenced = 0;
    my $lib = shift;
    my @external = ();

    open $obj, "objdump -T $lib|" || die "Can't objdump $lib: $!";

    while (<$obj>) {
	/[0-9a-f]*\s+([g ])\s+..\s+(\S*).....................\s+(.*)/ || next;
	
	my ($symbol, $type) = ($3, $2);

	if (!$symbol || !$type) {
	    next;
	}

	if ($symbol =~ /\w+C+[0-9]{1}/){
	#print "Symbol Name :: $symbol \n";
	 if ($type eq '.text') {
	    my $name = `c++filt $symbol`;
	    my @symb_arr = split /\(/, $name;
	    $name = $symb_arr[0];
	    $name =~ s/\:\:[^\:]*$//;

	    if (exists $symbols{$name}) {
		if($symbols{$name} ne $lib){
			print "$lib and $symbols{$name} defines '$name'\n";
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

opendir ($dirhandle, $path) || die "Can't open dir $path: $!";
while ($file = readdir ($dirhandle)) {
	if ($file =~ m/\S+.so/ &&
	    !($file =~ m/used/)) {
	    $libs{"$path/$file"} = undef;
	}
}
closedir ($dirhandle);

for $lib (keys %libs) {
    resolve_symbols ($lib);
}
