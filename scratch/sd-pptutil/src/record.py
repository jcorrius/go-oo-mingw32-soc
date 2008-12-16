
import struct
import globals

# -------------------------------------------------------------------
# record handler classes

class BaseRecordHandler(object):

    def __init__ (self, header, size, bytes):
        self.header = header
        self.size = size
        self.bytes = bytes
        self.lines = []
        self.pos = 0       # current byte position

    def parseBytes (self):
        """Parse the original bytes and generate human readable output.

The derived class should only worry about overwriting this function.  The
bytes are given as self.bytes, and call self.appendLine([new line]) to
append a line to be displayed.
"""
        pass

    def output (self):
        self.parseBytes()
        print("%4.4Xh: %s"%(self.header, "-"*61))
        for line in self.lines:
            print("%4.4Xh: %s"%(self.header, line))

    def appendLine (self, line):
        self.lines.append(line)

    def appendLineBoolean (self, name, value):
        text = "%s: %s"%(name, self.getYesNo(value))
        self.appendLine(text)

    def readBytes (self, length):
        r = self.bytes[self.pos:self.pos+length]
        self.pos += length
        return r

    def readRemainingBytes (self):
        r = self.bytes[self.pos:]
        self.pos = self.size
        return r

    def getCurrentPos (self):
        return self.pos

    def setCurrentPos (self, pos):
        self.pos = pos

    def getYesNo (self, boolVal):
        if boolVal:
            return 'yes'
        else:
            return 'no'

    def getTrueFalse (self, boolVal):
        if boolVal:
            return 'true'
        else:
            return 'false'

    def readUnsignedInt (self, length):
        bytes = self.readBytes(length)
        return globals.getUnsignedInt(bytes)

    def readSignedInt (self, length):
        bytes = self.readBytes(length)
        return globals.getSignedInt(bytes)

    def readDouble (self):
        # double is always 8 bytes.
        bytes = self.readBytes(8)
        return globals.getDouble(bytes)


class String(BaseRecordHandler):
    """Textual content."""

    def parseBytes (self):
        name = globals.getTextBytes(self.readRemainingBytes())
        self.appendLine("text: '%s'"%name)

class UniString(BaseRecordHandler):
    """Textual content."""

    def parseBytes (self):
        name = globals.getUTF8FromUTF16(globals.getTextBytes(self.readRemainingBytes()))
        self.appendLine("text: '%s'"%name)
