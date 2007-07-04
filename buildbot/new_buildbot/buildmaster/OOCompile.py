import os
from buildbot.steps.shell import ShellCommand
from buildbot.slave.registry import registerSlaveCommand
from buildbot.process.buildstep import LoggedRemoteCommand
# begin hackery
class OOCompile(ShellCommand):
      def createSummary(self, log):
        try:
            logFileName = self.step_status.logs[0].getFilename()

            command = "./create_logs.pl " + logFileName
	   

            result = os.popen(command).read()

            summary_log_file_name = logFileName + "_brief.html"
            summary_log_file = open(summary_log_file_name)

            self.addHTMLLog('summary log', summary_log_file.read())

            command = "grep warning: "+ logFileName
            warnings = os.popen(command).read()

            command = "grep error: "+ logFileName
            errors = os.popen(command).read()

            command = "tail -50 "+logFileName
            tail = os.popen(command).read()

            if warnings != "" :
                self.addCompleteLog('warnings',warnings)

            if errors != "":
                self.addCompleteLog('errors',errors)

            if tail != "":
                self.addCompleteLog('tail',tail)

        except:
            #log.msg("Exception: Cannot open logFile")
            print "cannot execute createSummary after OOCompile"
 
      def __init__(self, **kwargs):
          ShellCommand.__init__(self, **kwargs)   # always upcall!
