#!/usr/bin/perl -w

# Tool to help you to split a library into more ones.  Choose one or more
# objects (let's say <blah>.o) that you need in the library, and:
# 
# cd unxlng*.pro/slo
# for I in *.o ; do nm $I > ../symbols/${I/.o/.txt} ; done
# cd ../symbols
# [add the <blah>.txt's you need in the library to %closure (see below the escherex.txt as example)]
# ./split-library.pl
#
# Result: On stdout, you see all the objects you need in the same library

%undefined = ();
%where_defined = ();

sub read_symbols( $ )
{
    my ( $fname ) = @_;

    my @undefined = ();

    open FILE, "<$fname" || die "Cannot open $fname.\n";
    while ( <FILE> ) {
        if ( / [BTV] (.*)/ ) {
            $where_defined{$1} = $fname;
        }
        elsif ( / U (.*)/ ) {
            push @undefined, $1;
        }
    }
    close FILE;

    $undefined{$fname} = \@undefined;
}

my @files = split /\n/, `ls *.txt`;
foreach $file ( @files ) {
    read_symbols( $file ) if ( !( $file =~ /^\s*$/ ) );
}

%closure = ();

$closure{'escherex.txt'} = 1;
$closure{'eschesdo.txt'} = 1;

my $something_added = 0;
do {
    $something_added = 0;
    foreach $file ( keys %closure ) {
        my $undef_ref = $undefined{$file};

        foreach $undef ( @$undef_ref ) {
            my $where = $where_defined{$undef};
            if ( defined( $where ) && !defined( $closure{$where} ) ) {
                $closure{$where} = 1;
                $something_added = 1;
                print "$where\n";
            }
        }
    }
} while ( $something_added );
