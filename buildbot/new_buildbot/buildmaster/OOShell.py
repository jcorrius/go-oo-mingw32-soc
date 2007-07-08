from buildbot.steps.shell import ShellCommand
from buildbot.status.builder import SUCCESS, WARNINGS, FAILURE, SKIPPED
class OOShellCommand(ShellCommand):
  def __init__(self, **kwargs):
    ShellCommand.__init__(self, **kwargs)   # always upcall!

  def evaluateCommand(self, cmd):
    if cmd.rc == 65:        
      # this works, but step remains yellow forever.  <no reason given> never
      # seems to apear anywhere.  But the build does stop, which is good
      self.build.buildFinished(['slave rejected', '<no reason given'], 'green',
                               SKIPPED)
      return SKIPPED
    if cmd.rc != 0:
      return FAILURE
    # if cmd.log.getStderr(): return WARNINGS
    return SUCCESS

  def getText(self, cmd, results):
    if results == SUCCESS:
      return self.describe(True)
    elif results == WARNINGS:
      return self.describe(True) + ["warnings"]
    elif results == SKIPPED:
      return self.describe(True) + ["skipped"]
    else:
      return self.describe(True) + ["failed"]

  def getColor(self, cmd, results):
    assert results in (SUCCESS, WARNINGS, FAILURE, SKIPPED)
    if results == SUCCESS:
      return "green"
    elif results == WARNINGS:
      return "orange"
    elif results == SKIPPED:
      return "green"
    else:
      return "red"

