#!/usr/bin/perl -w

use XML::Parser;

my $offset = 0;
my $scale = 1.0;
my $unit = "cm";

sub get_point_attr
{
    my ($value, $name) = @_;
    return $value->[0]->{$name} / $scale + $offset;
}

sub parse_glue
{
    my $elems = shift;
    my $idx = 1;
    my $glue = '';
    my $count = 1;

    while (defined $elems->[$idx]) {
	my $attr = $elems->[$idx];
	my $value = $elems->[$idx+1];

	if ($attr eq 'point') {
	    my $x = get_point_attr($value, 'x');
	    my $y = get_point_attr($value, 'y');

	    $glue .= "<draw:glue-point draw:id=\"$count\" ".
		"svg:x=\"$x$unit\" svg:y=\"$y$unit\"/>\n";
	    $count++;
	}
#	print "glue elem '" . $elems->[$idx] . "'\n";
	$idx+=2;
    }

    return $glue;
}

sub parse_svg
{
    my $elems = shift;
    my $idx = 1;
    my $svg = '';
    my $count = 1;

    while (defined $elems->[$idx]) {
	my $attr = $elems->[$idx];
	my $value = $elems->[$idx+1];

	if ($attr eq 'svg:line') {
	    my $x1 = $value->[0]->{x1} / $scale + $offset;
	    my $y1 = $value->[0]->{y1} / $scale + $offset;
	}
#	print "svg elem '" . $elems->[$idx] . "'\n";
	$idx+=2;
    }

    return $svg;
}

sub parse_shape
{
    my $elems = shift;
    my $idx = 1;
    my $name = '';
    my $descr = '';
    my $shape = '';
    my $glue = '';

    while (defined $elems->[$idx]) {
	my $attr = $elems->[$idx];
	my $value = $elems->[$idx+1];
	
	$name = $value->[2]         if ($attr eq 'name');
	$descr = $value->[2]        if ($attr eq 'description');
	$glue = parse_glue ($value) if ($attr eq 'connections');
	$draw = parse_svg ($value)  if ($attr eq 'svg:svg');
	
#	print "elem '" . $elems->[$idx] . "'\n";
	$idx+=2;
    }

    print "$name: $descr\n";
    print "$glue\n";
}

my $file = shift;

die "Can't find file \"$file\""
  unless -f $file;

my $parser = new XML::Parser (Style => 'Tree');
my $tree = $parser->parsefile ($file) || die "Faield to parse\n";

$tree->[0] eq 'shape' || die "No shape\n";

parse_shape ($tree->[1]);
