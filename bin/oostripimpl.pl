#!/usr/bin/perl -w

my $libname = shift @ARGV;

$libname || die "Need a library name";

my $Symbols;

open ($Symbols, "objdump -T $libname |") || die "Can't objdump $libname: $!";

print "{\n\tlocal:\n";

while (<$Symbols>) {
    m/(\S+)$/ || next;

    my $line = $1;
    $line =~ m/Impl/ || next;

    print "\t$line;\n";
}

close ($Symbols) || die "Can't close: $!";

print "};\n";
