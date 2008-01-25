
import globals

#--------------------------------------------------------------------
# record handler classes

class BaseRecordHandler(object):

    def __init__ (self, header, size, bytes):
        self.header = header
        self.size = size
        self.bytes = bytes
        self.lines = []

    def parseBytes (self):
        pass

    def output (self):
        self.parseBytes()
        print("%4.4Xh: %s"%(self.header, "-"*61))
        for line in self.lines:
            print("%4.4Xh: %s"%(self.header, line))

    def appendLine (self, line):
        self.lines.append(line)


class BOF(BaseRecordHandler):

    Type = {
        0x0005: "Workbook globals",
        0x0006: "Visual Basic module",
        0x0010: "Sheet or dialog",
        0x0020: "Chart",
        0x0040: "Macro sheet",
        0x0100: "Workspace"
    }

    def __init__ (self, header, size, bytes):
        BaseRecordHandler.__init__(self, header, size, bytes)

    def parseBytes (self):
        ver = globals.getRawBytes(self.bytes[0:2])
        dataType = globals.getSignedInt(self.bytes[2:4])
        buildID = globals.getSignedInt(self.bytes[4:6])
        buildYear = globals.getSignedInt(self.bytes[6:8])
        fileHistoryFlags = globals.getRawBytes(self.bytes[8:12])
        lowestExcelVer = globals.getSignedInt(self.bytes[12:16])

        self.appendLine("BIFF version: " + ver)
        self.appendLine("build ID: %d"%buildID)
        self.appendLine("build year: %d"%buildYear)
        self.appendLine("type: %s"%BOF.Type[dataType])
        self.appendLine("file history flags: " + fileHistoryFlags)
        self.appendLine("lowest Excel version: %d"%lowestExcelVer)
        


class CTCellContent(BaseRecordHandler):

    def __init__ (self, header, size, bytes):
        BaseRecordHandler.__init__(self, header, size, bytes)

    def parseBytes (self):
        pass

    def output (self):
        BaseRecordHandler.output(self)



class CHChart(BaseRecordHandler):

    def __init__ (self, header, size, bytes):
        BaseRecordHandler.__init__(self, header, size, bytes)

    def parseBytes (self):
        x = globals.getSignedInt(self.bytes[0:4])
        y = globals.getSignedInt(self.bytes[4:8])
        w = globals.getSignedInt(self.bytes[8:12])
        h = globals.getSignedInt(self.bytes[12:16])
        self.appendLine("position: (x, y) = (%d, %d)"%(x, y))
        self.appendLine("size: (width, height) = (%d, %d)"%(w, h))
        

    def output (self):
        BaseRecordHandler.output(self)

