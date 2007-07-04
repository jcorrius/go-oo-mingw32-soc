#!/usr/bin/perl

use lib 'modules';

use Error_Parse;
use Utils;
use HTMLPopUp;
#use VCDisplay;

$log_file = shift;

sub set_filenames {

  # These are temporary files, used by the mail processsor and will be
  # erased before the program exits.  We use filenames which end in
  # html for debugging the mail processor in isolation.  We do not
  # glob on the prefix in the mailprocessor.


  %TMP_FILE = (
               'errorpick' => ("Tmp_errorpick.html"),
               
               'binaryfile' => ("Tmp_binaryfile.html"),

               'full-log' => ("Tmp_fulllog.html"),
               
               'brief-log' => ("Tmp_brieflog.html"),

              );


  %FILE = (

           # there is also a binaryfile created in assemble_files()
           # which gets created using information from get_filename()
           # and $TINDERBOX{}

           'full-log' => ($log_file."_full.html"),
           
           'brief-log' => ($log_file."_brief.html"),

           #'update_time_stamp' => (FileStructure::get_filename($tree, 'update_time_stamp').
           #                       ""),
              );

  #foreach $file ( (values %FILE), (values %TMP_FILE) ) {
  #  main::mkdir_R(dirname($file));
  #}

  # set the URL for how to get to the logfiles we will generate.

  # We can not end the cgibin calls in '.html.gz' or IE will think we
  # are returning a gziped file. Gunzip knows to add the extension and
  # to add the dirname to find the file.


  #$TINDERBOX{'brieflog'} = 
  #  ("$FileStructure::URLS{'gunzip'}?".
  #   "tree=$tree&brief-log=$main::UID");
  
  $TINDERBOX{'brieflog'} = 
    ("gunzip.cgi");
  

  $TINDERBOX{'fulllog'} = 
    ("gunzip.cgi");

  # record the basename of the log file (not used but useful for debugging.)

  #$TINDERBOX{'full-log'} = File::Basename::basename($FILE{'full-log'});
  #$TINDERBOX{'brief-log'} = File::Basename::basename($FILE{'brief-log'});
  
  return 1;
}



# create the HTML links for errors this is for both 
# 1) the $headerline,  which will appear in the error picklist 
#      at the top of the log file and points to the error 
#      in the same HTML file.
# 2) the $logline which is the actual error displayed as a HTML 
#     link which points to the CVS file and line which is the problem.


sub highlight_errors {
  my( $line, $line_type, $lineno, 
      $next_err_ref, $func_parse_errorline) = @_;
  
  my( @error ) = ();
  
  # clean up any embedded HTML in the mail
  $line = HTMLPopUp::escapeHTML($line);
  
  my ($logline) = '';
  my ($headerline) = '';
  
  # All log lines get line numbers so users can send mail pointing to
  # any line in the log file, example:
  #         Someone should look at the Makefile, this 
  #           make line <LINK> 
  #           is the same as this one <LINK>

  # line numbers start AFTER the tinderbox variables have been read.

  $logline .= HTMLPopUp::Link(
                              "name"=>$lineno,
                              "linktxt"=>$lineno,
                             "href"=>"\#$lineno",
                             );

  # add spaces so line numbers are left justified to 6 chars

  $logline .= (' ' x ($Error_Parse::LINENO_COLUMN-length($lineno)));

  if ( ( $line_type eq 'error' ) && ( !($last_was_error) ) ) {
    
    my ($err_ref) = $$next_err_ref;
    $$next_err_ref++;
    
    # tag this line and link to next error line
    
    $logline .= HTMLPopUp::Link(
                                "name"=>"err".($err_ref),
                                "linktxt"=>"NEXT", 
                                "href"=>"\#err".($$next_err_ref),
                               );
    
    $headerline .= HTMLPopUp::Link(
                                   "linktxt"=>$line, 
                                   "href"=>"\#err".($err_ref),
                                  );
    
  } else {
    
    # indent, so first column can be the 'next' link 
    
    $logline .= (" " x length("NEXT"));
    $headerline .= '';
    
  }
  
  # separate the logs from "NEXT" just a bit more.
  
  $logline .= "    ";
  
  # markup the line if it looks interesting
  
  if ($line_type ne 'info') {
    @error= &$func_parse_errorline($line);
    #if ("@error") {
    #  $line = VCDisplay::guess
    #    (
    #     'tree' => $TINDERBOX{'tree'}, 
    #     'file' => $error[0],
    #     'line' => $error[1],
    #     'linktxt' =>  $line, 
    #    );
    #}
    
    my $color = Error_Parse::type2color($line_type);
    ($color) &&
      ($color = "color=$color");

    $logline .=  ("<font $color>".
                  $line.
                  "</font>");
  } else {
    $logline .= $line;
  }
  
  
  return ($headerline, $logline);
}


sub process_bloat_data {
  my ($line, $func_line_type,) = @_;
  
  # we have to call &highlight_errors() even though there are no
  # errors, or the line numbers will get messed up
  
  my ($line_type) = &$func_line_type($line);
  @html = &highlight_errors($line, $line_type, $lineno, \$next_err, 
                            $func_parse_errorline);
  print FULL $html[1];
  if ( $line =~ m/^TOTAL/) {
    # Line format:
    #  TOTAL <absolute leaks> <% leaks delta> <absolute bloat> <% bloat delta>
    chomp;
    my ($leaks, $bloat) = (split /\s+/, $line)[1,3];
    my $bloat_string = (
                        "<br>Lk:". format_bloat_delta($leaks).
                        "<br>Bl:". format_bloat_delta($bloat,)
                       );
    
    $TINDERBOX{'bloatdata'} = $bloat_string;
  }
  return ;
}


sub parse_mail_body {

  # create several files with a single pass over the mail message:
  
  # BINARY:      the uuencoded file, if present, in the mailmessage.
  # FULL:        a complete copy of the logfile with HTML markup.
  # BRIEF:       contains only the error messages with some lines of 
  #                 surrounding context and HTML markup.
  # ERROR_PICK:  a list of html links to error messages, this will appear
  #                 on the top of both brief and full when our 
  #                 processing is complete. 

  # it is a bug in processing should this function be called before
  # the tinderbox variables are set. We intentionally leave off the \n
  # on some of the die's to get tracebacks of internal errors.

  # We must use HTMLPopUp::escapeHTML to clean up any embedded HTML in the
  # mail messege to prevent attacks.  Though it may be more friendly
  # to let compilers and things embed URLS in their error messages:

  # http://www.ciac.org/ciac/bulletins/k-021.shtml
  
  #   When a victim with scripts enabled in their browser reads this
  #   message, the malicious code may be executed
  #   unexpectedly. Scripting tags that can be embedded in this way
  #   include <SCRIPT>, <OBJECT>, <APPLET>, and <EMBED>.
  
  

  # It is a bit cleaner to convert these symbolic references to hard
  # references in the eval at the top of the function instead of
  # calling the symbolic reference to the function continually.  This
  # is also how we check that the function exists.

    my $func_line_type = '';
    my $func_parse_errorline = "";

    my ($out, $func);

    $func = "Error_Parse::unix::line_type";
    $out = eval "\$func_line_type = \\&${func};";
    ($@) &&
        die("Error:. $@");

    $func = "Error_Parse::unix::parse_errorline";
    $out = eval "\$func_parse_errorline = \\&${func};";
    ($@) &&
        die("Error:. $@");

  open(BINARY, ">$TMP_FILE{'binaryfile'}") ||
    die("Could not write to file: '$TMP_FILE{'binaryfile'}'. $!\n");
  open(FULL, ">$TMP_FILE{'full-log'}") ||
    die("Could not write to file: '$TMP_FILE{'full-log'}'. $!\n");
  open(BRIEF, ">$TMP_FILE{'brief-log'}") ||
    die("Could not write to file: '$TMP_FILE{'brief-log'}'. $!\n");
  open(ERROR_PICK, ">$TMP_FILE{'errorpick'}") ||
    die("Could not write to file: '$TMP_FILE{'errorpick'}'. $!\n");

  print ERROR_PICK "<!-- error pick menu -->\n<pre>\n";
  print ERROR_PICK HTMLPopUp::Link(
                                   "linktxt"=>"End of Log File", 
                                   "href"=>"\#EOF",
                                   ).
                                   "\n";
    

  my ($next_err) = 1;
  my ($lines_since_error) = 0;
  my ($error_lines) = 0;
  my (@lastlines) = ();

  open (LOGFILE, $log_file) || die "couldn't open the file!";

  while (defined($line = <LOGFILE>)) {

    $lineno++;

    my (@html) = '';

    # uuencoded binaries get pulled out into a special file

    if ( ( $line =~ m/^begin [0-7][0-7][0-7] / ) ..  
         ( $line =~ m/^end\n/ ) 
       ) {

      print BINARY $line;
      next;
    }

    # Find and save the TinderboxPrint data

    if ($line =~ m/\s*TinderboxPrint\s*:\s*(.*)/) {
        my $print = $1;

        # Long lines might get split by mailing software but then we
        # can't reconstitute them, since we will not know how many
        # lines to take.  Make sure refs, if they exist, are closed;

        my $good_line = (
                         ($print !~ m!<\s*a\s+!i) ||
                         ($print =~ m!>[^<>]*<\s*/a\s*>!i)
                         );

        if ($good_line) {
            $TINDERBOX{'print'} .= "&nbsp; ".$print;
        }

        print FULL $line;
        next;
    }

    # process bloat statistics 

    if ( ($line =~ m/^\#+ BLOAT STATISTICS/) ..  
         ( $line =~ m/^--NEW-BLOAT--/ ) 
       ) {
      process_bloat_data($line,$func_line_type,);
      next;
    }

    $lines_since_error++;

    # prepare the line for browser display
    my ($line_type) = &$func_line_type($line);
    my %args = (
                'lineno' => $lineno, 
                'line_type' => $line_type, 
                'line' => $line,
                );

    Error_Parse::run_status_handler(%TINDERBOX, %args);

    @html = &highlight_errors($line, $line_type, $lineno, \$next_err, 
                                   $func_parse_errorline);


    print FULL $html[1];

    # brief log processing

    if ($html[0]) {

      my ($lines_context) = min( ($lines_since_error - $Error_Parse::LINES_AFTER_ERROR - 2),
                                 $Error_Parse::LINES_BEFORE_ERROR);
      my ($context_index) = max($#lastlines - $lines_context, 0);
      my ($lines_skiped)  = max ( $lines_since_error - 
                                  ($Error_Parse::LINES_BEFORE_ERROR + $Error_Parse::LINES_AFTER_ERROR+1), 
                                  0);
      
      print ERROR_PICK $html[0];

      if ($lines_skiped) {
        print BRIEF ("\n<i><font size=\"+1\">".
                     " Skipping $lines_skiped Lines...".
                     "</font></i>\n\n");

      }

      print BRIEF @lastlines[$context_index .. $#lastlines];

      $print_to_brief = 1 + $Error_Parse::LINES_AFTER_ERROR;

      $lines_since_error = 0;
      $error_lines++;
    } 

    if ($print_to_brief) {
      print BRIEF $html[1];
      $print_to_brief--;
    }
    

    push @lastlines, $html[1];
    
    # don't keep more lines of history then we need
    
    if ( !( $#lastlines & 127 ) ){
      my ($first) = $#lastlines - $Error_Parse::LINES_BEFORE_ERROR; 
      my ($last) = $#lastlines;
      @lastlines = @lastlines[ $first .. $last ];      
    }

  } # while
  
  close(LOGFILE);

  # We always want to see the very last line of the build, whether or
  # not the build failed.  So treat the last line like an error and
  # print the context surrounding it.

  my ($lines_context) = min( ($lines_since_error - $Error_Parse::LINES_AFTER_ERROR - 2),
                             $Error_Parse::LINES_BEFORE_ERROR);
  my ($context_index) = max($#lastlines - $lines_context, 0);
  my ($lines_skiped)  = max ( $lines_since_error - 
                              ($Error_Parse::LINES_BEFORE_ERROR + $Error_Parse::LINES_AFTER_ERROR+1), 
                              0);
      
  print ERROR_PICK $line;

  if ($lines_skiped) {
    print BRIEF ("\n<i><font size=\"+1\">".
                 " Skipping $lines_skiped Lines...".
                 "</font></i>\n\n");
    
  }
  
  print BRIEF @lastlines[$context_index .. $#lastlines];

  my ($last_errline) = ("</pre><p>".
                        "<font size=\"+1\">".
                        HTMLPopUp::Link(
                                        "name"=>"err".($next_err_ref),
                                        "linktxt"=>"No More Errors", 
                                        "href"=>"\#err".(1),
                                        ).
                        "</font>".
                        HTMLPopUp::Link(
                                        "name"=>"EOF",
                                        ).
                        "<br>");
  
  print BRIEF $last_errline;
  print FULL  $last_errline;
  
  print ERROR_PICK "\n";
  print ERROR_PICK "<!-- end error pick menu -->\n";
  print ERROR_PICK "\n";
  print ERROR_PICK "<H2>Build Log</H2>\n";
  print ERROR_PICK "<pre>\n";
  print ERROR_PICK "\n\n";

  close(BINARY) ||
    die("Could not close file: '$TMP_FILE{'binaryfile'}'. $!\n");
  close(FULL) ||                 
    die("Could not close file: '$TMP_FILE{'full-log'}'. $!\n");
  close(BRIEF) ||                
    die("Could not close file: '$TMP_FILE{'brief-log'}'. $!\n");
  close(ERROR_PICK) ||           
    die("Could not close file: '$TMP_FILE{'errorpick'}'. $!\n");

  # store the count of errors
  $TINDERBOX{'errors'} = $error_lines;


  return ;  
}


sub assemble_files {

# piece together the permanent files from the temporary ones.

# much of this could be done in shell scripts (cat) but we have more
# error control in perl and it is more portable to non-unix OS.

  #my $tree = $TINDERBOX{'tree'};


  {

    # append the headers to the top of the full log and compress the result
    
    open(ERROR_PICK, "<$TMP_FILE{'errorpick'}") ||
      die("Could not read from file: '$TMP_FILE{'errorpick'}'.\n");
    open(TMP_FULL, "<$TMP_FILE{'full-log'}") ||
      die("Could not read from file: '$TMP_FILE{'full-log'}'.\n");
    open(FULL, ">$FILE{'full-log'}") ||
      die("Could not write to file: '$FILE{'full-log'}'.\n");
    
    #print FULL log_header('full');
    
    while (defined($line=<ERROR_PICK>)) {
      print FULL $line;
    }
    
    while (defined($line=<TMP_FULL>)) {
      print FULL $line;
    }
    
    #print FULL log_footer('full');
    
    close(FULL) ||
      die("Could not close file: '$FILE{'full-log'}': waitstatus: $? : $! \n");
    close(TMP_FULL) ||
      die("Could not close file: '$TMP_FILE{'full-log'}': $! \n");
    close(ERROR_PICK) ||
      die("Could not close file: '$TMP_FILE{'errorpick'}': $! \n");
  }
  
  {

    # append the headers to the top of the brief log

    open(ERROR_PICK, "<$TMP_FILE{'errorpick'}") ||
      die("Could not read from file: '$TMP_FILE{'errorpick'}'.\n");
    open(TMP_BRIEF, "<$TMP_FILE{'brief-log'}") ||
      die("Could not read from file: '$TMP_FILE{'brief-log'}'.\n");
    open(BRIEF, ">$FILE{'brief-log'}") ||
      die("Could not write to file: '$FILE{'brief-log'}'.\n");
    
    #print BRIEF log_header('brief');

    while (defined($line=<ERROR_PICK>)) {
      print BRIEF $line;
    }
    while (defined($line=<TMP_BRIEF>)) {
      print BRIEF $line;
    }
    
    #print BRIEF log_footer('brief');

    close(BRIEF) ||
      die("Could not close file: '$FILE{'brief-log'}'.\n");
    close(TMP_BRIEF) ||            
      die("Could not close file: '$TMP_FILE{'brief-log'}'.\n");
    close(ERROR_PICK) ||           
      die("Could not close file: '$TMP_FILE{'errorpick'}'.\n");
    
  }
  
  if( 
     ($TINDERBOX{'binaryname'}) && 
     (!(-z $TMP_FILE{'binaryfile'})) 
    ) {
    my $bin_dir = "bin_dir";
    $bin_dir =~ s/ //g;
    
    mkdir_R($bin_dir);
    
    $TINDERBOX{'binaryname'} = File::Basename::basename($TINDERBOX{'binaryname'});
    my $outfile = "$bin_dir/".$TINDERBOX{'binaryname'};

    my (@cmd) = (
                 @UUDECODE,
                 $outfile,
                 $TMP_FILE{'binaryfile'},
                );

    system(@cmd) ||
      die("Could not run cmd: '@cmd'. $!\n");
    
    # the builder tells us the basename of the binary, we tell the
    # server the full path name of the binary.

  }

  #{
  #    my $cmd = "gzip -f ".$FILE{'full-log'} ; 
  #	  print("gzip command: $cmd\n");
  #	  system($cmd) && die "Can't do GZIP operation";
  #}

  #{
  #    my $cmd = "gzip -f ".$FILE{'brief-log'} ; 
  #	  print("gzip command: $cmd\n");
  #	  system($cmd) && die "Can't do GZIP operation";
  #}

  return ;
}



#---------------- main ----------------------
{

  %TINDERBOX = ();
  set_filenames();
  parse_mail_body();
  assemble_files();

  # clean up all the temp files created in this script
    
  foreach $tmpfile (values %TMP_FILE) {

    (!(-e $tmpfile)) ||
      unlink($tmpfile) ||
        die("Could not unlink: '$tmpfile'. $!\n");    

  }

  print $OUT;

  exit 0;
}
