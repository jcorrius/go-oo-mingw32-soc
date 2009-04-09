#!/usr/bin/perl -w

# This script changes the definite article of ProductName

use strict;

my $sdffile1 = $ARGV[0]; 
if (!$sdffile1) {die "Usage: a2az.pl <SDF file>\n";}

         #        (                          leftpart                                                     )        (         rightpart                                   )    
         #           prj      file     dummy     type      gid        lid        helpid   pform     width     lang      text     helptext  qhelptext  title    timestamp
my $sdf_regex  = "((([^\t]*)\t([^\t]*)\t([^\t]*)\t([^\t]*)\t([^\t]*)\t([^\t]*)\t([^\t])*\t([^\t]*)\t([^\t]*))\t([^\t]*)\t(([^\t]*)\t([^\t]*)\t([^\t]*)\t([^\t]*)\t)([^\t]*))";


open FILE1 , "< $sdffile1" or die "Can't open '$sdffile1'\n";

while( <FILE1>){
         if( /$sdf_regex/ ){
            my $lang           = defined $12 ? $12 : '';
            my $prj            = defined $3 ? $3 : '';
            if ($lang eq "hu") {
              if ($prj ne "databaseext" && 
                  $prj ne "embeddedobj" && 
                  $prj ne "extras_full" && 
                  $prj ne "instset_native" && 
                  $prj ne "lingu" && 
                  $prj ne "macromigration" && 
                  $prj ne "migrationanalysis" && 
                  $prj ne "offmgr" && 
                  $prj ne "readme" && 
                  $prj ne "setup2" && 
                  $prj ne "syncaccess" && 
                  $prj ne "tab" && 
                  $prj ne "scp2so" && 
                  $prj ne "customres") {
                   s/([ 	~">][Aa]) %PRODUCTNAME/$1z %PRODUCTNAME/g;
                   s/([ 	~">][Aa]) \\<emph\\>%PRODUCTNAME/$1z \\<emph\\>%PRODUCTNAME/g;
                   s/([ 	~">][Aa]) \\<item type=\\\"productname\\\"\\>%PRODUCTNAME/$1z \\<item type=\\\"productname\\\"\\>%PRODUCTNAME/g;
                   s/([ 	~">][Aa]) %productname/$1z %productname/g;
                   s/([ 	~">][Aa]) \$\[officename\]/$1z \$\[officename\]/g;
                   s/([ 	~">][Aa]) \$\{PRODUCT_NAME\}/$1z \$\{PRODUCT_NAME\}/g;
                   s/([ 	~">][Aa]) \\<emph\\>\$\[officename\]/$1z \\<emph\\>\$\[officename\]/g;
                   s/([ 	~">][Aa]) \[ProductName\]/$1z \[ProductName\]/g;
                   s/\\nA %PRODUCTNAME/\\nAz %PRODUCTNAME/g;
              }
            };
         }
         print $_;
}

close(FILE1);

exit 0;

