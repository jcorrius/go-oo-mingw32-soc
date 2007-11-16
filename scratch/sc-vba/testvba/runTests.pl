#!/usr/bin/perl -w
use URI::Escape;
use File::Basename;
use Cwd;
use Cwd 'abs_path';

my $binDir = abs_path( dirname($0) );

my $sysDir = "unix";
my $fileSep = "/";
my $theResult;
my $officepath = shift || die "please specify path to office installation program dir";
my $binext = "";
my $testDocDir = "$binDir/TestDocuments";
my $testLogDir = "$binDir/Logs";

if ( open(UNAME, "uname -a|") ) {
   $theResult = <UNAME>; 
   close(UNAME);
   if (  $theResult =~ /^CYGWIN/  ) {
      # windows under cygwin
      $sysDir = "win" ;
      $tmpPath=$ENV{"PATH"};
      $ENV{"PATH"} = "$tmpPath:$officepath";
      $testDocDir=`cygpath -m  $testDocDir`;
      uri_escape($testDocDir);
      # hacky windows url construction
      $testDocDir="file:///$testDocDir";
      
      chomp($testDocDir);
      #print "*** doc dir is $testDocDir\n";
      $testLogDir = `cygpath -m  "$testLogDir"`;
      uri_escape($testLogDir);
      $testLogDir="file:///$testLogDir";
      chomp($testLogDir);
      #print "*** log dir is $testLogDir\n";
      $binext = ".exe";
   }
   else{
      # unix we need to find sal etc. ( from the office path )
      my $tmpPATH = $ENV{"LD_LIBRARY_PATH"};
      $ENV{"LD_LIBRARY_PATH"} = "$tmpPATH:$officepath";
   }
}
else
{
      # ordinary windows, not sure if this will actually work
      $sysDir = "win" ;
      $ENV{"PATH"} = "$PATH;$officepath";
      $binext = ".exe";
}

# the exe needs system paths or urls ( urls are by far the least troublesome )

my $runCmd = "$binDir/testclient$binext $testDocDir $testLogDir";
my $analyseCmd = "$binDir/testResults.pl $binDir/Logs $binDir/TestDocuments/logs/$sysDir";
print "runCmd = $runCmd\n";

my $status = system( $runCmd );
print "analyseCmd = $analyseCmd\n";
$status = system( $analyseCmd );
