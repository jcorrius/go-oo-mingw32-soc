
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

        try:
            self.appendLine("BIFF version: " + ver)
            self.appendLine("build ID: %d"%buildID)
            self.appendLine("build year: %d"%buildYear)
            self.appendLine("type: %s"%BOF.Type[dataType])
            self.appendLine("file history flags: " + fileHistoryFlags)
            self.appendLine("lowest Excel version: %d"%lowestExcelVer)
        except:
            pass



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



class Name(BaseRecordHandler):
    """internal defined name"""

    def __getInt (self, offset, size):
        return globals.getSignedInt(self.bytes[offset:offset+size])

    def __parseOptionFlags (self, flags):
        self.appendLine("option flags:")
        isHidden       = (flags & 0x0001) != 0
        isFuncMacro    = (flags & 0x0002) != 0
        isVBMacro      = (flags & 0x0004) != 0
        isMacroName    = (flags & 0x0008) != 0
        isComplFormula = (flags & 0x0010) != 0
        isBuiltinName  = (flags & 0x0020) != 0
        funcGrp        = (flags & 0x0FC0) / 64
        isBinary       = (flags & 0x1000) != 0

        if isHidden:
            self.appendLine("  hidden")
        else:
            self.appendLine("  visible")

        if isMacroName:
            self.appendLine("  macro name")
            if isFuncMacro:
                self.appendLine("  function macro")
                self.appendLine("  function group: %d"%funcGrp)
            else:
                self.appendLine("  command macro")
            if isVBMacro:
                self.appendLine("  visual basic macro")
            else:
                self.appendLine("  sheet macro")
        else:
            self.appendLine("  standard name")

        if isComplFormula:
            self.appendLine("  complex formula")
        else:
            self.appendLine("  simple formula")
        if isBuiltinName:
            self.appendLine("  built-in name")
        else:
            self.appendLine("  user-defined name")
        if isBinary:
            self.appendLine("  binary data")
        else:
            self.appendLine("  formula definition")


    def parseBytes (self):
        optionFlags = self.__getInt(0, 2)

        keyShortCut = self.__getInt(2, 1)
        nameLen     = self.__getInt(3, 1)
        formulaLen  = self.__getInt(4, 2)
        sheetId     = self.__getInt(8, 2)

        # these optional texts may come after the formula token bytes.
        menuTextLen = self.__getInt(10, 1)
        descTextLen = self.__getInt(11, 1)
        helpTextLen = self.__getInt(12, 1)
        statTextLen = self.__getInt(13, 1)

        name, byteLen = globals.getRichText(self.bytes[14:], nameLen)
        name = globals.decodeName(name)
        tokenPos = 14 + byteLen
        tokenText = globals.getRawBytes(self.bytes[tokenPos:tokenPos+formulaLen], True, False)
        o = formula.FormulaParser(self.bytes[tokenPos:tokenPos+formulaLen], False)
        o.parse()
        self.appendLine("name: %s"%name)
        self.__parseOptionFlags(optionFlags)

        self.appendLine("sheet ID: %d"%sheetId)
        self.appendLine("menu text length: %d"%menuTextLen)
        self.appendLine("description length: %d"%descTextLen)
        self.appendLine("help tip text length: %d"%helpTextLen)
        self.appendLine("status bar text length: %d"%statTextLen)
        self.appendLine("formula length: %d"%formulaLen)
        self.appendLine("formula bytes: " + tokenText)
        self.appendLine("formula: " + o.getText())



class SupBook(BaseRecordHandler):
    """Supporting workbook"""

    def __parseSpecial (self):
        if self.bytes[2:4] == [0x01, 0x04]:
            # internal reference
            num = globals.getSignedInt(self.bytes[0:2])
            self.appendLine("sheet name count: %d (internal reference)"%num)
        elif self.bytes[0:4] == [0x00, 0x01, 0x01, 0x3A]:
            # add-in function
            self.appendLine("add-in function name stored in the following EXERNNAME record.")

    def __parseDDE (self):
        # not implemented yet.
        pass

    def parseBytes (self):
        if self.size == 4:
            self.__parseSpecial()
            return

        if self.bytes[0:2] == [0x00, 0x00]:
            self.__parseDDE()
            return

        num = globals.getSignedInt(self.bytes[0:2])
        self.appendLine("sheet name count: %d"%num)
        i = 2
        isFirst = True
        while i < self.size:
            nameLen = globals.getSignedInt(self.bytes[i:i+2])
            i += 2
            flags = globals.getSignedInt(self.bytes[i:i+1])
            i += 1
            name = globals.getTextBytes(self.bytes[i:i+nameLen])
            name = globals.decodeName(name)
            i += nameLen
            if isFirst:
                isFirst = False
                self.appendLine("document URL: %s"%name)
            else:
                self.appendLine("sheet name: %s"%name)


class ExternSheet(BaseRecordHandler):

    def parseBytes (self):
        num = globals.getSignedInt(self.bytes[0:2])
        for i in xrange(0, num):
            offset = 2 + i*6
            book = globals.getSignedInt(self.bytes[offset:offset+2])
            firstSheet = globals.getSignedInt(self.bytes[offset+2:offset+4])
            lastSheet  = globals.getSignedInt(self.bytes[offset+4:offset+6])
            self.appendLine("SUPBOOK record ID: %d  (sheet ID range: %d - %d)"%(book, firstSheet, lastSheet))


class ExternName(BaseRecordHandler):

    def __parseOptionFlags (self, flags):
        self.isBuiltinName = (flags & 0x0001) != 0
        self.isAutoDDE     = (flags & 0x0002) != 0
        self.isStdDocName  = (flags & 0x0008) != 0
        self.isOLE         = (flags & 0x0010) != 0
        # 5 - 14 bits stores last successful clip format
        self.lastClipFormat = (flags & 0x7FE0)

    def parseBytes (self):
        # first 2 bytes are option flags for external name.
        optionFlags = globals.getSignedInt(self.bytes[0:2])
        self.__parseOptionFlags(optionFlags)

        if self.isOLE:
            # next 4 bytes are 32-bit storage ID
            storageID = globals.getSignedInt(self.bytes[2:6])
            nameLen = globals.getSignedInt(self.bytes[6:7])
            name, byteLen = globals.getRichText(self.bytes[7:], nameLen)
            self.appendLine("type: OLE")
            self.appendLine("storage ID: %d"%storageID)
            self.appendLine("name: %s"%name)
        else:
            # assume external defined name (could be DDE link).
            # TODO: differentiate DDE link from external defined name.

            supbookID = globals.getSignedInt(self.bytes[2:4])
            nameLen = globals.getSignedInt(self.bytes[6:7])
            name, byteLen = globals.getRichText(self.bytes[7:], nameLen)
            tokenText = globals.getRawBytes(self.bytes[7+byteLen:], True, False)
            self.appendLine("type: defined name")
            if supbookID == 0:
                self.appendLine("sheet ID: 0 (global defined names)")
            else:
                self.appendLine("sheet ID: %d"%supbookID)
            self.appendLine("name: %s"%name)
            self.appendLine("formula bytes: %s"%tokenText)

            # parse formula tokens
            o = formula.FormulaParser(self.bytes[7+byteLen:])
            o.parse()
            ftext = o.getText()
            self.appendLine("formula: %s"%ftext)


class Xct(BaseRecordHandler):

    def parseBytes (self):
        crnCount = globals.getSignedInt(self.bytes[0:2])
        sheetIndex = globals.getSignedInt(self.bytes[2:4])
        self.appendLine("CRN count: %d"%crnCount)
        self.appendLine("index of referenced sheet in the SUPBOOK record: %d"%sheetIndex)

class Crn(BaseRecordHandler):

    def parseBytes (self):
        lastCol = globals.getSignedInt(self.bytes[0:1])
        firstCol = globals.getSignedInt(self.bytes[1:2])
        rowIndex = globals.getSignedInt(self.bytes[2:4])
        self.appendLine("first column: %d"%firstCol)
        self.appendLine("last column:  %d"%lastCol)
        self.appendLine("row index: %d"%rowIndex)

        i = 4
        n = len(self.bytes)
        while i < n:
            typeId = self.bytes[i]
            i += 1
            if typeId == 0x00:
                # empty value
                i += 8
                self.appendLine("* empty value")
            elif typeId == 0x01:
                # number
                val = globals.getDouble(self.bytes[i:i+8])
                i += 8
                self.appendLine("* numeric value (%g)"%val)
            elif typeId == 0x2:
                # string
                text, length = globals.getRichText(self.bytes[i:])
                i += length
                text = globals.decodeName(text)
                self.appendLine("* string value (%s)"%text)
            elif typeId == 0x04:
                # boolean
                val = self.bytes[i]
                i += 7 # next 7 bytes not used
                self.appendLine("* boolean value (%d)"%val)
            elif typeId == 0x10:
                # error value
                val = self.bytes[i]
                i += 7 # not used
                self.appendLine("* error value (%d)"%val)
            else:
                sys.stderr.write("error parsing CRN record")
                sys.exit(1)
            



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
        
