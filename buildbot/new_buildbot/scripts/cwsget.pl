#!/usr/bin/env perl
##
## cwsget.pl
## Started on  Thu Mar 29 12:05:44 2007 Michael Leibowitz
## $Id$
## 
## Copyright (C) 2007 Intel Corporation
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##

use warnings;
use strict;
use LWP::UserAgent;
use Getopt::Long;
use Switch;

use Data::Dumper;

use File::Spec;

my $DEBUG= 0;

my @all_dirs= split / /, 'accessibility animations autodoc automation avmedia basctl basegfx basic bean beanshell berkeleydb binfilter bitstream_vera_fonts boost bridges canvas chart2 cli_ure codemaker comphelper configmgr config_office connectivity cosv cppcanvas cppu cppuhelper cpputools crashrep curl dbaccess default_images desktop dictionaries dmake dtrans embeddedobj embedserv epm eventattacher expat extensions external extras fileaccess filter forms fpicker framework freetype goodies helpcontent2 hsqldb hwpfilter i18npool i18nutil icu idlc idl instsetoo_native io javaunohelper jpeg jurt jut jvmaccess jvmfwk libwpd libxml2 libxmlsec lingucomponent linguistic MathMLDTD moz msfontextract nas neon netbeans_integration np_sdk odk offapi officecfg offuh ooo_custom_images package padmin portaudio postprocess psprint_config psprint python pyuno qadevOOo rdbmaker readlicense_oo regexp registry remotebridges rhino ridljar rsc rvpapi sablot sal salhelper sandbox sane sax scaddins sc sch scp2 scripting sd sdk_oo setup_native sfx2 shell sj2 slideshow smoketestoo_native sndfile so3 solenv soltools sot starmath stlport stoc store svtools svx sw sysui testshl2 testshl testtools toolkit tools transex3 twain ucb ucbhelper udkapi udm unixODBC UnoControls unoil unotools unoxml unzip uui vcl vos wizards writerperfect x11_extensions xalan xmerge xml2cmp xmlhelp xmloff xmlscript XmlSearch xmlsecurity zlib ure unodevtools drawinglayer fondu agg external_images o3tl vigra basebmp libegg lpsolve scsolver exthome libtextcat writerfilter javainstaller2 afms xt oovbaapi';

sub run
{
    if ($DEBUG)
    {
	print "system(\"" . join ('", "', @_) . "\");\n";
    }
    else
    {
	die "can't run $_[0]!!\n" unless !system(@_);
    }
}

sub valid_mode($)
{
    my $mode= shift;
    return 1 if ($mode eq 'up') || ($mode eq 'co');
    return 0;
}

#return a list of items that are in @a but not in @b
sub array_difference($$)
{
    my $a= shift;
    my $b= shift;

    my @c;
    my %B;
    foreach (@{$b}) { $B{$_}= 1; }
    foreach (@{$a})	
    {
 	push @c, $_ unless $B{$_};
    }
    return @c;
}

sub grab_tag_list
{
    my $url= 'http://go-oo.org/tinderbox/tags/tag-list';
    my $ua = LWP::UserAgent->new;
        
    my $response = $ua->get($url);
    unless ($response->is_success) { die $response->status_line; }
    my %h;
    foreach(split /\n/, $response->content)
    {
	unless (/\#.*/)
	{
	    (my $name, my $base, my $tag, my $dirs) = split / : /, $_;
	    
	    $h{$name}= 
	    {
		'base' => $base,
		'tag'  => $tag,
		'dirs' => [split / /, $dirs]
		};
	}
    }
    return \%h;
}

sub grab($$$$$)
{
    my $hash= shift;
    my $cvs_root= shift;
    my $cvs_options= shift;
    my $mode= shift;
    my $rev= shift;

    die "cws not found!!\n" unless ($hash->{$rev});
    
    my $base_tag= $hash->{$rev}->{'base'};
    my $cws_tag=  $hash->{$rev}->{'tag'};
    
    my $mode_cmd;
    switch($mode)
    {
	case 'co' { $mode_cmd= 'co -P'; }
	case 'up' { $mode_cmd= 'up -dP'; }
	else { die "invalid mode $mode!!!\n"; }
    }

    my @dirs_not_in_cws= array_difference(\@all_dirs, $hash->{$rev}->{'dirs'});
					  
    foreach (@dirs_not_in_cws)
    {
	run('cvs', "-d", "$cvs_root", "$cvs_options", 'co', "-r", "$base_tag", "$_");
    }
    foreach (@{$hash->{$rev}->{'dirs'}})
    {
	run('cvs', "-d", "$cvs_root", "$cvs_options", 'co', "-r", "$cws_tag", "$_");
    }
}


# Main
my $help_option;
my $cvs_root= '';
my $cvs_options= '';
my $mode= '';
my $rev= '';
my $dir= '';

GetOptions('cvsopts=s'        => \$cvs_options,
	   'mode=s'           => \$mode,
	   'cvsroot=s'        => \$cvs_root,
	   'revision=s'       => \$rev,
	   'dir=s'            => \$dir,
	   'help'             => \$help_option);

if ($help_option
    || ($cvs_root eq '')
    || !valid_mode($mode)
    || ($rev eq ''))
       
{
    print STDERR "$0 usage: --mode=<up/co> --cvsroot=<cvs root>" . 
	" --rev=<branch/mileston> [--cvsopts=<cvsopts>] [--dir=<dir>]\n\n";
    exit 1;
}
if ($dir)
{

my $volume;
my $dirs;
my $file;
($volume, $dirs, $file) = File::Spec->splitpath(File::Spec->rel2abs(File::Spec->curdir()));
print $file;
print "\n";
    if ($dir ne $file) {
        unless (-d $dir)
        {
	       mkdir($dir) || die "Couldn't mkdir \"$dir\"!\n";
        }
        chdir($dir) || die "Couldn't change to directory \"$dir\"!\n";
    }
}
my $hashref= grab_tag_list();
grab($hashref, $cvs_root, $cvs_options, $mode, $rev);
exit 0;



