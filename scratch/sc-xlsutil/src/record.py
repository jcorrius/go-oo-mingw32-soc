
import globals

# -------------------------------------------------------------------
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

# --------------------------------------------------------------------

class BOF(BaseRecordHandler):

    Type = {
        0x0005: "Workbook globals",
        0x0006: "Visual Basic module",
        0x0010: "Sheet or dialog",
        0x0020: "Chart",
        0x0040: "Macro sheet",
        0x0100: "Workspace"
    }

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


class Number(BaseRecordHandler):

    def parseBytes (self):
        row = globals.getSignedInt(self.bytes[0:2])
        col = globals.getSignedInt(self.bytes[2:4])
        xf  = globals.getSignedInt(self.bytes[4:6])
        fval = globals.getDouble(self.bytes[6:14])
        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)
        self.appendLine("value (IEEE 754): %g"%fval)

# -------------------------------------------------------------------
# CT - Change Tracking

class CTCellContent(BaseRecordHandler):
    pass

# -------------------------------------------------------------------
# CH - Chart

class CHChart(BaseRecordHandler):

    def parseBytes (self):
        x = globals.getSignedInt(self.bytes[0:4])
        y = globals.getSignedInt(self.bytes[4:8])
        w = globals.getSignedInt(self.bytes[8:12])
        h = globals.getSignedInt(self.bytes[12:16])
        self.appendLine("position: (x, y) = (%d, %d)"%(x, y))
        self.appendLine("size: (width, height) = (%d, %d)"%(w, h))
        
