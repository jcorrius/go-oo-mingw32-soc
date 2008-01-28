
import struct
import globals, formula

# -------------------------------------------------------------------
# record handler classes

class BaseRecordHandler(object):

    def __init__ (self, header, size, bytes):
        self.header = header
        self.size = size
        self.bytes = bytes
        self.lines = []

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


class Formula(BaseRecordHandler):

    def parseBytes (self):
        row  = globals.getSignedInt(self.bytes[0:2])
        col  = globals.getSignedInt(self.bytes[2:4])
        xf   = globals.getSignedInt(self.bytes[4:6])
        fval = globals.getDouble(self.bytes[6:14])

        flags          = globals.getSignedInt(self.bytes[14:16])
        recalc         = (flags & 0x0001) != 0
        calcOnOpen     = (flags & 0x0002) != 0
        sharedFormula  = (flags & 0x0008) != 0

        tokens = self.bytes[20:]
        fparser = formula.FormulaParser(tokens)
        fparser.parse()
        ftext = fparser.getText()

        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)
        self.appendLine("formula result: %g"%fval)
        self.appendLine("recalculate always: %d"%recalc)
        self.appendLine("calculate on open: %d"%calcOnOpen)
        self.appendLine("shared formula: %d"%sharedFormula)
        self.appendLine("tokens: "+ftext)


class Number(BaseRecordHandler):

    def parseBytes (self):
        row = globals.getSignedInt(self.bytes[0:2])
        col = globals.getSignedInt(self.bytes[2:4])
        xf  = globals.getSignedInt(self.bytes[4:6])
        fval = globals.getDouble(self.bytes[6:14])
        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)
        self.appendLine("value: %g"%fval)


class RK(BaseRecordHandler):
    """Cell with encoded integer or floating-point value"""

    def parseBytes (self):
        row = globals.getSignedInt(self.bytes[0:2])
        col = globals.getSignedInt(self.bytes[2:4])
        xf  = globals.getSignedInt(self.bytes[4:6])

        rkval = globals.getSignedInt(self.bytes[6:10])
        multi100  = ((rkval & 0x00000001) != 0)
        signedInt = ((rkval & 0x00000002) != 0)
        realVal   = (rkval & 0xFFFFFFFC)

        if signedInt:
            # for integer, perform right-shift by 2 bits.
            realVal = realVal/4
        else:
            # for floating-point, convert the value back to the bytes,
            # pad the bytes to make it 8-byte long, and convert it back
            # to the numeric value.
            tmpBytes = struct.pack('<L', realVal)
            tmpBytes = struct.pack('xxxx') + tmpBytes
            realVal = struct.unpack('<d', tmpBytes)[0]

        if multi100:
            realVal /= 100

        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)
        self.appendLine("multiplied by 100: %d"%multi100)
        if signedInt:
            self.appendLine("type: signed integer")
        else:
            self.appendLine("type: floating point")
        self.appendLine("value: %g"%realVal)


class Blank(BaseRecordHandler):

    def parseBytes (self):
        row = globals.getSignedInt(self.bytes[0:2])
        col = globals.getSignedInt(self.bytes[2:4])
        xf  = globals.getSignedInt(self.bytes[4:6])
        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)


class Row(BaseRecordHandler):

    def parseBytes (self):
        row  = globals.getSignedInt(self.bytes[0:2])
        col1 = globals.getSignedInt(self.bytes[2:4])
        col2 = globals.getSignedInt(self.bytes[4:6])

        rowHeightBits = globals.getSignedInt(self.bytes[6:8])
        rowHeight     = (rowHeightBits & 0x7FFF)
        defaultHeight = ((rowHeightBits & 0x8000) == 1)

        self.appendLine("row: %d; col: %d - %d"%(row, col1, col2))
        self.appendLine("row height (twips): %d"%rowHeight)
        if defaultHeight:
            self.appendLine("row height type: default")
        else:
            self.appendLine("row height type: custom")


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
        
