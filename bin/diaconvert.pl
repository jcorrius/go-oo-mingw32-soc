#!/usr/bin/perl -w

use XML::Parser;

my $X_OFFSET_MIN = 2;
my $X_OFFSET_MAX = 8;

my $xoffset = $X_OFFSET_MIN;
my $yoffset = 2;
my $scale = 5;
my $unit = "cm";
my $prefix = '      ';

sub get_size_attr($$)
{
    my ($value, $name) = @_;
    return $value->[0]->{$name} / $scale if defined $value->[0]->{$name};
    return 0.0;
}

sub get_point_attr($$)
{
    my ($value, $name) = @_;
    my $offset = $yoffset;
    if ($name =~ /x/) {
	$offset = $xoffset;
    }
    return get_size_attr ($value, $name) + $offset;
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

	    $glue .= $prefix . '  ';
	    $glue .= "<draw:glue-point draw:id=\"$count\" ".
		"svg:x=\"$x$unit\" svg:y=\"$y$unit\"/>\n";
	    $count++;
	}
#	print "glue elem '" . $elems->[$idx] . "'\n";
	$idx+=2;
    }

    return $glue;
}

sub draw_preamble($@)
{
    my $name = shift;
    my $style = shift;
    $style = 'def' if (!defined $style);
    return $prefix . '  ' .
	    "<draw:$name draw:style-name=\"$style\" " .
	    "draw:text-style-name=\"P1\" " .
	    "draw:layer=\"layout\"\n" .
	    $prefix . '  ' . ' ';
}

sub transfer_attr($$)
{
    my ($value, $name) = @_;
    my $attr = get_point_attr ($value, $name);
    return "svg:$name=\"$attr$unit\" ";
}

sub draw_postamble($)
{
    my $name = shift;
    return ">\n" . $prefix . '  ' . "<text:p/></draw:$name>\n";
}

sub draw_line($$)
{
    my ($attr, $value) = @_;
    return draw_preamble ('line') .
	    transfer_attr ($value, 'x1') .
	    transfer_attr ($value, 'y1') .
	    transfer_attr ($value, 'x2') .
	    transfer_attr ($value, 'y2') .
	    draw_postamble ('line');
}

sub draw_rect($$)
{
    my ($attr, $value) = @_;
    my $svg;
    my $width = get_size_attr ($value, 'width');
    my $height = get_size_attr ($value, 'height');
    return draw_preamble ('rect', 'nofill') .
	    transfer_attr ($value, 'x') .
	    transfer_attr ($value, 'y') .
	    "svg:width=\"$width$unit\" " .
	    "svg:height=\"$height$unit\" " .
	    draw_postamble ('rect');
}

sub draw_circle($$)
{
    my ($attr, $value) = @_;
    my $r  = get_size_attr ($value, 'r'); 
    my $x = get_point_attr ($value, 'cx') - $r;
    my $y = get_point_attr ($value, 'cy') - $r;
    my $size = $r * 2;

    return draw_preamble ('circle', 'nofill') .
	    "svg:width=\"$size$unit\" " .
	    "svg:height=\"$size$unit\" " .
	    "svg:x=\"$x$unit\" " .
	    "svg:y=\"$y$unit\" " .
	    draw_postamble ('circle');
}

sub adapt_bbox($$$)
{
    my ($data, $x, $y) = @_;

    $data->{minx} = $x if ($x < $data->{minx});
    $data->{miny} = $y if ($y < $data->{miny});
    $data->{maxx} = $x if ($x > $data->{maxx});
    $data->{maxy} = $y if ($y > $data->{maxy});
}

sub draw_poly($$)
{
    my ($attr, $value) = @_;
    my $svg = '';
    my $src_points = $value->[0]->{points};
    my $points = '';
    my %bbox = ( maxx => 0, maxy => 0,
		 minx => 10000000, miny => 10000000 );

    print $maxx;

    for my $coord (split / /, $src_points) {
	my ($a, $b) = split /,/, $coord;
	my $x = ($a / $scale + $xoffset) * 1000;
	my $y = ($b / $scale + $yoffset) * 1000;
	
	$points .= "$x,$y ";
	adapt_bbox (\%bbox, $x, $y);
    }

    my $viewbox = "0 0 " . $maxx . " " . $maxy;
    
    $attr =~ s/^svg://;
    $svg .= draw_preamble ($attr, 'nofill') .
	    "svg:x=\"" . $minx/1000 . "$unit\" " .
	    "svg:y=\"" . $miny/1000 . "$unit\" " .
	    "svg:width=\"" . $maxx/1000 . "$unit\" " .
	    "svg:height=\"" . $maxy/1000 . "$unit\" " .
	    "svg:viewBox=\"$viewbox\" " .
	    "draw:points=\"$points\" " .
	    draw_postamble ($attr);

    return $svg;
}

sub draw_path
{
    my ($attr, $value) = @_;
    my $svg = '';
    my $path = $value->[0]->{'d'};

# ensure adequate field separation
    $path =~ s/[lL]/ l /g;
    $path =~ s/[mM]/ m /g;
    $path =~ s/[cC]/ c /g;
    $path =~ s/[zZ]/ z /g;
    $path =~ s/-/ -/g;

    $path =~ s/^\s*//;
    my @elems = split (/ +/, $path);

    my %bbox = ( maxx => 0, maxy => 0,
		 minx => 10000000, miny => 10000000 );
    while (my $elem = shift @elems) {
	if ($elem eq 'm') {
	    adapt_bbox (\%bbox, $x, $y);
	} elsif ($elem eq 'l') {
	} elsif ($elem eq 'c') {
	} elsif ($elem eq 'z') {
	    printf STDERR "What does 'z' do ?\n";
	}
    }

    print STDERR "Path '@elems'\n";
    
#    print STDERR "No svg:path handling\n";
# path is 'M\s+<x>\s+<y>
# L<x><y>
# C<x1y1> <x2y2> <x3y3>
# 'z'
#    viewbox calculation ?!? - guess it ? expand bits ?
    return $svg;
}

%draw_handlers = (
    'svg:rect'     => \&draw_rect,
    'svg:polygon'  => \&draw_poly,
    'svg:polyline' => \&draw_poly,
    'svg:line'     => \&draw_line,
    'svg:circle'   => \&draw_circle,
    'svg:path'     => \&draw_path
);

sub parse_svg
{
    my $elems = shift;
    my $idx = 1;
    my $svg = '';
    my $count = 1;

    while (defined $elems->[$idx]) {
	my $attr = $elems->[$idx];
	my $value = $elems->[$idx+1];
	my $draw = $draw_handlers{$attr};
	
	if (defined $draw) {
	    $svg .= $draw->($attr, $value);

	} elsif ($attr ne '0') {
	    print STDERR "unknown svg elem '" . $elems->[$idx] . "'\n";
	}
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
	$idx += 2;
    }

    print <<"EOS"
$prefix<draw:g draw:name="$name">
$glue
$draw
$prefix</draw:g>
EOS
;
}

sub output_header
{
print <<'EOS'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE office:document-content PUBLIC "-//OpenOffice.org//DTD OfficeDocument 1.0//EN" "office.dtd">
<office:document-content xmlns:office="http://openoffice.org/2000/office" xmlns:style="http://openoffice.org/2000/style" xmlns:text="http://openoffice.org/2000/text" xmlns:table="http://openoffice.org/2000/table" xmlns:draw="http://openoffice.org/2000/drawing" xmlns:fo="http://www.w3.org/1999/XSL/Format" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:number="http://openoffice.org/2000/datastyle" xmlns:presentation="http://openoffice.org/2000/presentation" xmlns:svg="http://www.w3.org/2000/svg" xmlns:chart="http://openoffice.org/2000/chart" xmlns:dr3d="http://openoffice.org/2000/dr3d" xmlns:math="http://www.w3.org/1998/Math/MathML" xmlns:form="http://openoffice.org/2000/form" xmlns:script="http://openoffice.org/2000/script" office:class="drawing" office:version="1.0">
  <office:script/>
  <office:automatic-styles>
    <style:style style:name="dp1" style:family="drawing-page"/>
    <style:style style:name="def" style:family="graphics" style:parent-style-name="standard">
      <style:properties draw:textarea-horizontal-align="center" draw:textarea-vertical-align="middle"/>
    </style:style>
    <style:style style:name="nofill" style:family="graphics" style:parent-style-name="objectwithoutfill">
      <style:properties draw:fill="none" draw:textarea-horizontal-align="center" draw:textarea-vertical-align="middle"/>
    </style:style>
    <style:style style:name="P1" style:family="paragraph">
      <style:properties fo:text-align="center"/>
    </style:style>
  </office:automatic-styles>
  <office:body>
    <draw:page draw:name="page1" draw:style-name="dp1" draw:master-page-name="Default">
EOS
;
}

sub output_footer
{
print <<'EOS'
    </draw:page>
  </office:body>
</office:document-content>
EOS
;
}

sub output_shape($)
{
    my $file = shift;

    my $parser = new XML::Parser (Style => 'Tree');
    my $tree = $parser->parsefile ($file) || die "Faield to parse\n";
    
    $tree->[0] eq 'shape' || die "No shape\n";
    parse_shape ($tree->[1]);
}

output_header ();

while (my $file = shift @ARGV)
{
    die "Can't find file \"$file\""
	unless -f $file;
    output_shape ($file);
    $xoffset += 2.0;
    if ($xoffset > $X_OFFSET_MAX) {
	$xoffset = $X_OFFSET_MIN;
	$yoffset += 2.0
    }
}

output_footer ();
