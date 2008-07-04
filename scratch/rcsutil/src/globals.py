
import sys

class Debuggable(object):

    def __init__ (self):
        self.debug = True

    def debugPrint (self, msg, abortAfter=False):
        if self.debug:
            sys.stderr.write(msg + "\n")
            if abortAfter:
                sys.exit(1)
