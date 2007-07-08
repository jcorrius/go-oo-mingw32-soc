#!/usr/bin/perl -w

use File::Basename;
use File::stat;
use File::Copy;
#sub gen_diff($)
sub testLog
{
   my $passed = 1;
   my $testfile = shift;
   my $dirtocheck = shift;
   my $filename = basename($testfile);
   $filename = "$logdir/$filename"; 
   if ( -f $filename )  {
      my $tmpFile = "/tmp/gen_diff"; 
      my $status = system("diff -up $testfile $filename |  $cvsclean > $tmpFile");
      my $info = stat($tmpFile) or die "no $tmpFile: $!";
      if ( ($status >>=8) == 0 &&  ( $info->size > 0)  ) {
         $passed = 0; 
      }
   }
   else
   {
      $passed = 0;
   }
   print "result $passed\n";
   return $passed;
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
print "logdir $testlogdir\n";
sub filter_crud($)
{
    my $a = shift;

    $a =~ /~$/ && return;
    $a =~ /\#$/ && return;
    $a =~ /\.orig$/ && return;
    $a =~ /unxlng.*\.pro$/ && return;
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
	if (-d "$dir/$fname") {
	    push @files, slurp_dir("$dir/$fname");
	} else {
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
for $a (@files) {
   $processed++;
   if ( testLog( $a, $logdir ) == 0 ) {
      print "Test document for $a \t \t failed.\n";
   }
   else {
      $passed++;
      print "Test document for $a \t \t passed. \n"; 
   } 
}
print "processed \t $processed documents\n";
print "\t \t $passed tests passed\n";
