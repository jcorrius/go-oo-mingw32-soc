#!/usr/bin/perl -w

use File::Basename;
use File::stat;
use File::Copy;
my $binDir = dirname($0);
my $timestampclean= "$binDir/timestampsClean.pl";
#sub gen_diff($)

sub testLog
{
   # 2 No Log to compare against
   # 1 Log passed 
   # 0 Log failed
   my $result = 1;
   my $testfile = shift;
   my $dirtocheck = shift;
   my $filename = basename($testfile);
   $filename = "$logdir/$filename"; 
   if ( -f $filename )  {
      my $tmpFile = "/tmp/gen_diff"; 
      my $status = system("diff -up $testfile $filename |  $timestampclean > $tmpFile");
      my $info = stat($tmpFile) or die "no $tmpFile: $!";
      if ( ($status >>=8) == 0 &&  ( $info->size > 0)  ) {
         $result = 0; 
      }
   }
   else
   {
      $result = 1;
   }
   return $result;
}

if ( ! ( $logdir = shift @ARGV ) ) {
    print STDERR "No logdir specified!\n";
    usage();
    exit 1;
}

if ( ! ( $testlogdir = shift @ARGV ) ) {
    print STDERR "No testdocuments dir to compare against specified!\n";
    usage();
    exit 1;
}

print "logdir $logdir\n";
print "testlogdir $testlogdir\n";
$testlogdir = "$testlogdir/logs/ooo";
sub filter_crud($)
{
    my $a = shift;

    $a =~ /~$/ && return;
    $a =~ /\#$/ && return;
    $a =~ /\.orig$/ && return;
    $a =~ /unxlng.*\.pro$/ && return;
    $a =~ /wntmsc.*\.pro$/ && return;
    $a =~ /.swp$/ && return;
    $a =~ /POSITION/ && return;
    $a =~ /ReadMe/ && return;
    $a =~ /.tmp$/ && return;
    $a =~ /\.svn/ && return;
    $a eq 'CVS' && return;
    $a eq '.' && return;
    $a eq '..' && return;

    return $a;
}
sub slurp_dir($);

sub slurp_dir($)
{
    my $dir = shift;
    my ($dirhandle, $fname);
    my @files = ();

    opendir ($dirhandle, $dir) || die "Can't open $dir";
    while ($fname = readdir ($dirhandle)) {
	$fname = filter_crud($fname);
	defined $fname || next;
#	if (-d "$dir/$fname") {
#	    push @files, slurp_dir("$dir/$fname");
#	} else 
        {
	    push @files, "$dir/$fname";
	}
    }
    closedir ($dirhandle);

    return @files;
}

if (-d $testlogdir) {
    push @files, slurp_dir($testlogdir);
}

my $processed = 0;
my $passed = 0;
my @passedTests=();
my @skippedTests=();
my @failedTests=();

my $failureCmd="";
for $a (@files) {
   $processed++;
   my $testcase = $a;  
   $testcase =~ s/\.log/\.xls/;
   my $result = testLog( $a, $logdir );
   if ( $result == 0 ) {
      push @failedTests, basename($testcase);
      if ( $failureCmd eq "" ) { $failureCmd = " diff -up $a $logdir "; }
   }
   elsif ( $result == 2 ) {
      #print "skipped $a\n";
      push @skippedTests, $testcase;
   } 
   else {
      $passed++;
      push @passedTests, $testcase;
      #print "Test document for $a \t \t passed. \n"; 
   } 
}
my $compared=@passedTests+@failedTests;
my $skip = @skippedTests;
print "skipped $skip test-cases(s)\n";
print "compared $compared test-case documents\n";
print "\t \t $passed tests $@passedTests\n";
if ( @failedTests > 0 ) {
   print "the following test-case documents failed, please examine the logs manually\n";
   
   for $a (@failedTests) {
      print "\t$a\n";
   }
   print "e.g. $failureCmd\n"
}
