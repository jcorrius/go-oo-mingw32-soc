#!/usr/bin/perl -w

my @output_buffer = ();
my $fname;
my $detectedSomeGuff = 0;
sub pure_guff($)
{
  my $array = shift;
  my @lines = @{$array};
  my $contains_sense = '';
  my $contains_guff = '';
  while (scalar @lines)
  {
    my $line = pop @lines;
    if ($line =~ m/Test run started :/ ||
	$line =~ m/Test run finished :/) {
      $contains_guff = '1';
    } elsif ($line =~ m/^[\+\-][^\-\+]/) {
      $contains_sense = '1';
    }
  }
  if ($contains_guff && $contains_sense) {
    print STDERR "Patch fragment with mixed good/bad changes in '$ARGV' near $line_index\n";
    $contains_guff = '';
  }
  elsif ( $contains_guff ) {
	$detectedSomeGuff = 1;
  }
#  print "contains guff: $contains_guff\n";
  return $contains_guff;
}

sub output_lines($)
{
  my $array = shift;
  my @lines = @{$array};

  if (pure_guff (\@lines)) {
    return;
  }

  while (scalar @lines)
  {
    my $line = pop @lines;
    push @output_buffer, $line;
  }
}

my $header;
my @lines;
my $frag_count = 0;
$line_index = 0;

while (<>) {
  if (/^\@\@/ || /^[^ \-\+]/) {
    output_lines (\@lines);
    @lines = ();
    $frag_count++;
  }
  unshift @lines, $_;
  $line_index++;
  close ARGV if eof;
}
output_lines(\@lines);

# basically if the two files compared
# have the values for the rcid type info
# then the first diff is ignored.
# hence fragstocount is set to 1 if there is any guff ( rcid head bits )

my $fragstocount = 0;
if ( $detectedSomeGuff ) {
   $fragstocount = 2;
}
if ($frag_count > $fragstocount) {
  print @output_buffer;
}
