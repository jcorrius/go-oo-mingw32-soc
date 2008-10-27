
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

    def readBytes (self, length):
        r = self.bytes[self.pos:self.pos+length]
        self.pos += length
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
        fparser = formula.FormulaParser(self.header, tokens)
        fparser.parse()
        ftext = fparser.getText()

        self.appendLine("cell position: (col: %d; row: %d)"%(col, row))
        self.appendLine("XF record ID: %d"%xf)
        self.appendLine("formula result: %g"%fval)
        self.appendLine("recalculate always: %d"%recalc)
        self.appendLine("calculate on open: %d"%calcOnOpen)
        self.appendLine("shared formula: %d"%sharedFormula)
        self.appendLine("formula bytes: %s"%globals.getRawBytes(tokens, True, False))
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


class String(BaseRecordHandler):
    """Cached string formula result for preceding formula record."""

    def parseBytes (self):
        strLen = globals.getSignedInt(self.bytes[0:1])
        name, byteLen = globals.getRichText(self.bytes[2:], strLen)
        self.appendLine("string value: '%s'"%name)


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
        o = formula.FormulaParser(self.header, self.bytes[tokenPos:tokenPos+formulaLen], False)
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
            o = formula.FormulaParser(self.header, self.bytes[7+byteLen:])
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
            

class RefreshAll(BaseRecordHandler):

    def parseBytes (self):
        boolVal = globals.getSignedInt(self.bytes[0:2])
        strVal = "no"
        if boolVal:
            strVal = "yes"
        self.appendLine("refresh all external data ranges and pivot tables: %s"%strVal)


# -------------------------------------------------------------------
# SX - Pivot Table

class SXViewEx9(BaseRecordHandler):

    def parseBytes (self):
        rt = globals.getSignedInt(self.bytes[0:2])
        self.appendLine("record type: 0x%4.4X (always 0x0810)"%rt)
        autoFmtId = globals.getSignedInt(self.bytes[12:14])
        self.appendLine("autoformat index: %d"%autoFmtId)


class SXAddlInfo(BaseRecordHandler):

    sxcNameList = {
        0x00: "sxcView",
        0x01: "sxcField",
        0x02: "sxcHierarchy",
        0x03: "sxcCache",
        0x04: "sxcCacheField",
        0x05: "sxcQsi",
        0x06: "sxcQuery",
        0x07: "sxcGrpLevel",
        0x08: "sxcGroup"
    }

    sxdNameList = {
        0x00: 'sxdId',
        0x01: 'sxdVerUpdInv',
        0x02: 'sxdVer10Info',
        0x03: 'sxdCalcMember',
        0x04: 'sxdXMLSource',
        0x05: 'sxdProperty',
        0x05: 'sxdSrcDataFile',
        0x06: 'sxdGrpLevelInfo',
        0x06: 'sxdSrcConnFile',
        0x07: 'sxdGrpInfo',
        0x07: 'sxdReconnCond',
        0x08: 'sxdMember',
        0x09: 'sxdFilterMember',
        0x0A: 'sxdCalcMemString',
        0xFF: 'sxdEnd'
    }

    def parseBytes (self):
        dummy = self.readBytes(2) # 0x0864
        dummy = self.readBytes(2) # 0x0000
        sxc = self.readBytes(1)[0]
        sxd = self.readBytes(1)[0]
        dwUserData = self.readBytes(4)
        dummy = self.readBytes(2)

        className = "(unknown)"
        if SXAddlInfo.sxcNameList.has_key(sxc):
            className = SXAddlInfo.sxcNameList[sxc]
        self.appendLine("class name: %s"%className)
        typeName = '(unknown)'
        if SXAddlInfo.sxdNameList.has_key(sxd):
            typeName = SXAddlInfo.sxdNameList[sxd]
        self.appendLine("type name: %s"%typeName)
        
        if sxd == 0x00:
            self.__parseId(sxc, dwUserData)

        elif sxd == 0x02:
            if sxc == 0x03:
                self.__parseSxDbSave10()
            elif sxc == 0x00:
                self.__parseViewFlags(dwUserData)

    def __parseViewFlags (self, dwUserData):
        flags = globals.getUnsignedInt(dwUserData)
        viewVer = (flags & 0x000000FF)
        verName = self.__getExcelVerName(viewVer)
        self.appendLine("PivotTable view version: %s"%verName)
        displayImmediateItems = (flags & 0x00000100)
        enableDataEd          = (flags & 0x00000200)
        disableFList          = (flags & 0x00000400)
        reenterOnLoadOnce     = (flags & 0x00000800)
        notViewCalcMembers    = (flags & 0x00001000)
        notVisualTotals       = (flags & 0x00002000)
        pageMultiItemLabel    = (flags & 0x00004000)
        tensorFillCv          = (flags & 0x00008000)
        hideDDData            = (flags & 0x00010000)

        self.appendLine("display immediate items: %s"%self.getYesNo(displayImmediateItems))
        self.appendLine("editing values in data area allowed: %s"%self.getYesNo(enableDataEd))
        self.appendLine("field list disabled: %s"%self.getYesNo(disableFList))
        self.appendLine("re-center on load once: %s"%self.getYesNo(reenterOnLoadOnce))
        self.appendLine("hide calculated members: %s"%self.getYesNo(notViewCalcMembers))
        self.appendLine("totals include hidden members: %s"%self.getYesNo(notVisualTotals))
        self.appendLine("(Multiple Items) instead of (All) in page field: %s"%self.getYesNo(pageMultiItemLabel))
        self.appendLine("background color from source: %s"%self.getYesNo(tensorFillCv))
        self.appendLine("hide drill-down for data field: %s"%self.getYesNo(hideDDData))

    def __parseId (self, sxc, dwUserData):
        if sxc == 0x03:
            idCache = globals.getUnsignedInt(dwUserData)
            self.appendLine("cache ID: %d"%idCache)
        elif sxc in [0x00, 0x01, 0x02, 0x05, 0x06, 0x07, 0x08]:
            lenStr = globals.getUnsignedInt(dwUserData)
            self.appendLine("length of ID string: %d"%lenStr)
            textLen = globals.getUnsignedInt(self.readBytes(2))
            data = self.bytes[self.getCurrentPos():]
            if lenStr == 0:
                self.appendLine("name (ID) string: (continued from last record)")
            elif lenStr == len(data) - 1:
                text, textLen = globals.getRichText(data, textLen)
                self.appendLine("name (ID) string: %s"%text)
            else:
                self.appendLine("name (ID) string: (first of multiple records)")


    def __parseSxDbSave10 (self):
        countGhostMax = globals.getSignedInt(self.readBytes(4))
        self.appendLine("max ghost pivot items: %d"%countGhostMax)

        # version last refreshed this cache
        lastVer = globals.getUnsignedInt(self.readBytes(1))
        verName = self.__getExcelVerName(lastVer)
        self.appendLine("last version refreshed: %s"%verName)
        
        # minimum version needed to refresh this cache
        lastVer = globals.getUnsignedInt(self.readBytes(1))
        verName = self.__getExcelVerName(lastVer)
        self.appendLine("minimum version needed to refresh: %s"%verName)

        # date last refreshed
        dateRefreshed = globals.getDouble(self.readBytes(8))
        self.appendLine("date last refreshed: %g"%dateRefreshed)


    def __getExcelVerName (self, ver):
        verName = '(unknown)'
        if ver == 0:
            verName = 'Excel 9 (2000) and earlier'
        elif ver == 1:
            verName = 'Excel 10 (XP)'
        elif ver == 2:
            verName = 'Excel 11 (2003)'
        elif ver == 3:
            verName = 'Excel 12 (2007)'
        return verName




class SXStreamID(BaseRecordHandler):

    def parseBytes (self):
        if self.size != 2:
            return

        strmId = globals.getSignedInt(self.bytes)
        self.appendLine("pivot cache stream ID in SX DB storage: %d"%strmId)

class SXViewSource(BaseRecordHandler):

    def parseBytes (self):
        if self.size != 2:
            return

        src = globals.getSignedInt(self.bytes)
        srcType = 'unknown'
        if src == 0x01:
            srcType = 'Excel list or database'
        elif src == 0x02:
            srcType = 'External data source (Microsoft Query)'
        elif src == 0x04:
            srcType = 'Multiple consolidation ranges'
        elif src == 0x10:
            srcType = 'Scenario Manager summary report'

        self.appendLine("data source type: %s"%srcType)

class SXViewFields(BaseRecordHandler):

    def parseBytes (self):
        axis          = globals.getSignedInt(self.readBytes(2))
        subtotalCount = globals.getSignedInt(self.readBytes(2))
        subtotalType  = globals.getSignedInt(self.readBytes(2))
        itemCount     = globals.getSignedInt(self.readBytes(2))
        nameLen       = globals.getSignedInt(self.readBytes(2))
        
        axisType = 'unknown'
        if axis == 0:
            axisType = 'no axis'
        elif axis == 1:
            axisType = 'row'
        elif axis == 2:
            axisType = 'column'
        elif axis == 4:
            axisType = 'page'
        elif axis == 8:
            axisType = 'data'

        subtotalTypeName = 'unknown'
        if subtotalType == 0x0000:
            subtotalTypeName = 'None'
        elif subtotalType == 0x0001:
            subtotalTypeName = 'Default'
        elif subtotalType == 0x0002:
            subtotalTypeName = 'Sum'
        elif subtotalType == 0x0004:
            subtotalTypeName = 'CountA'
        elif subtotalType == 0x0008:
            subtotalTypeName = 'Average'
        elif subtotalType == 0x0010:
            subtotalTypeName = 'Max'
        elif subtotalType == 0x0020:
            subtotalTypeName = 'Min'
        elif subtotalType == 0x0040:
            subtotalTypeName = 'Product'
        elif subtotalType == 0x0080:
            subtotalTypeName = 'Count'
        elif subtotalType == 0x0100:
            subtotalTypeName = 'Stdev'
        elif subtotalType == 0x0200:
            subtotalTypeName = 'StdevP'
        elif subtotalType == 0x0400:
            subtotalTypeName = 'Var'
        elif subtotalType == 0x0800:
            subtotalTypeName = 'VarP'

        self.appendLine("axis type: %s"%axisType)
        self.appendLine("number of subtotals: %d"%subtotalCount)
        self.appendLine("subtotal type: %s"%subtotalTypeName)
        self.appendLine("number of items: %d"%itemCount)

        if nameLen == -1:
            self.appendLine("name length: null (use name in the cache)")
        else:
            self.appendLine("name length: %d"%nameLen)

class SXViewItem(BaseRecordHandler):

    itemTypes = {
        0xFE: 'Page',
        0xFF: 'Null',
        0x00: 'Data',
        0x01: 'Default',
        0x02: 'SUM',
        0x03: 'COUNTA',
        0x04: 'COUNT',
        0x05: 'AVERAGE',
        0x06: 'MAX',
        0x07: 'MIN',
        0x08: 'PRODUCT',
        0x09: 'STDEV',
        0x0A: 'STDEVP',
        0x0B: 'VAR',
        0x0C: 'VARP',
        0x0D: 'Grand total',
        0x0E: 'blank'
    }

    def parseBytes (self):
        itemType = globals.getSignedInt(self.readBytes(2))
        grbit    = globals.getSignedInt(self.readBytes(2))
        iCache   = globals.getSignedInt(self.readBytes(2))
        nameLen  = globals.getSignedInt(self.readBytes(2))
        
        itemTypeName = 'unknown'
        if SXViewItem.itemTypes.has_key(itemType):
            itemTypeName = SXViewItem.itemTypes[itemType]

        flags = ''
        if (grbit & 0x0001):
            flags += 'hidden, '
        if (grbit & 0x0002):
            flags += 'detail hidden, '
        if (grbit & 0x0008):
            flags += 'formula, '
        if (grbit & 0x0010):
            flags += 'missing, '

        if len(flags) > 0:
            # strip the trailing ', '
            flags = flags[:-2]
        else:
            flags = '(none)'

        self.appendLine("item type: %s"%itemTypeName)
        self.appendLine("flags: %s"%flags)
        self.appendLine("pivot cache index: %d"%iCache)
        if nameLen == -1:
            self.appendLine("name length: null (use name in the cache)")
        else:
            self.appendLine("name length: %d"%nameLen)


class PivotQueryTableEx(BaseRecordHandler):

    def parseBytes (self):
        pass


# -------------------------------------------------------------------
# CT - Change Tracking

class CTCellContent(BaseRecordHandler):
    
    EXC_CHTR_TYPE_MASK       = 0x0007
    EXC_CHTR_TYPE_FORMATMASK = 0xFF00
    EXC_CHTR_TYPE_EMPTY      = 0x0000
    EXC_CHTR_TYPE_RK         = 0x0001
    EXC_CHTR_TYPE_DOUBLE     = 0x0002
    EXC_CHTR_TYPE_STRING     = 0x0003
    EXC_CHTR_TYPE_BOOL       = 0x0004
    EXC_CHTR_TYPE_FORMULA    = 0x0005

    def parseBytes (self):
        size = globals.getSignedInt(self.readBytes(4))
        id = globals.getSignedInt(self.readBytes(4))
        opcode = globals.getSignedInt(self.readBytes(2))
        accept = globals.getSignedInt(self.readBytes(2))
        tabCreateId = globals.getSignedInt(self.readBytes(2))
        valueType = globals.getSignedInt(self.readBytes(2))
        self.appendLine("header: (size=%d; index=%d; opcode=0x%2.2X; accept=%d)"%(size, id, opcode, accept))
        self.appendLine("sheet creation id: %d"%tabCreateId)

        oldType = (valueType/(2*2*2) & CTCellContent.EXC_CHTR_TYPE_MASK)
        newType = (valueType & CTCellContent.EXC_CHTR_TYPE_MASK)
        self.appendLine("value type: (old=0x%4.4X; new=0x%4.4X)"%(oldType, newType))
        self.readBytes(2) # ignore next 2 bytes.

        row = globals.getSignedInt(self.readBytes(2))
        col = globals.getSignedInt(self.readBytes(2))
        cell = formula.CellAddress(col, row)
        self.appendLine("cell position: %s"%cell.getName())

        oldSize = globals.getSignedInt(self.readBytes(2))
        self.readBytes(4) # ignore 4 bytes.

        fmtType = (valueType & CTCellContent.EXC_CHTR_TYPE_FORMATMASK)
        if fmtType == 0x1100:
            self.readBytes(16)
        elif fmtType == 0x1300:
            self.readBytes(8)

        self.readCell(oldType, "old cell type")
        self.readCell(newType, "new cell type")

    def readCell (self, cellType, cellName):

        cellTypeText = 'unknown'

        if cellType == CTCellContent.EXC_CHTR_TYPE_FORMULA:
            cellTypeText, formulaBytes, formulaText = self.readFormula()
            self.appendLine("%s: %s"%(cellName, cellTypeText))
            self.appendLine("formula bytes: %s"%globals.getRawBytes(formulaBytes, True, False))
            self.appendLine("tokens: %s"%formulaText)
            return

        if cellType == CTCellContent.EXC_CHTR_TYPE_EMPTY:
            cellTypeText = 'empty'
        elif cellType == CTCellContent.EXC_CHTR_TYPE_RK:
            cellTypeText = self.readRK()
        elif cellType == CTCellContent.EXC_CHTR_TYPE_DOUBLE:
            cellTypeText = self.readDouble()
        elif cellType == CTCellContent.EXC_CHTR_TYPE_STRING:
            cellTypeText = self.readString()
        elif cellType == CTCellContent.EXC_CHTR_TYPE_BOOL:
            cellTypeText = self.readBool()
        elif cellType == CTCellContent.EXC_CHTR_TYPE_FORMULA:
            cellTypeText, formulaText = self.readFormula()

        self.appendLine("%s: %s"%(cellName, cellTypeText))

    def readRK (self):
        valRK = globals.getSignedInt(self.readBytes(4))
        return 'RK value'

    def readDouble (self):
        val = globals.getDouble(self.readBytes(4))
        return "value %f"%val

    def readString (self):
        size = globals.getSignedInt(self.readBytes(2))
        pos = self.getCurrentPos()
        name, byteLen = globals.getRichText(self.bytes[pos:], size)
        self.setCurrentPos(pos + byteLen)
        return "string '%s'"%name

    def readBool (self):
        bool = globals.getSignedInt(self.readBytes(2))
        return "bool (%d)"%bool

    def readFormula (self):
        size = globals.getSignedInt(self.readBytes(2))
        fmlaBytes = self.readBytes(size)
        o = formula.FormulaParser(self.header, fmlaBytes, False)
        o.parse()
        return "formula", fmlaBytes, o.getText()









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
        

class CHProperties(BaseRecordHandler):

    def parseBytes (self):
        flags = globals.getSignedInt(self.bytes[0:2])
        emptyFlags = globals.getSignedInt(self.bytes[2:4])

        manualSeries   = "false"
        showVisCells   = "false"
        noResize       = "false"
        manualPlotArea = "false"

        if (flags & 0x0001):
            manualSeries = "true"
        if (flags & 0x0002):
            showVisCells = "true"
        if (flags & 0x0004):
            noResize = "true"
        if (flags & 0x0008):
            manualPlotArea = "true"

        self.appendLine("manual series: %s"%manualSeries)
        self.appendLine("show only visible cells: %s"%showVisCells)
        self.appendLine("no resize: %s"%noResize)
        self.appendLine("manual plot area: %s"%manualPlotArea)

        emptyValues = "skip"
        if emptyFlags == 1:
            emptyValues = "plot as zero"
        elif emptyFlags == 2:
            emptyValues = "interpolate empty values"

        self.appendLine("empty value treatment: %s"%emptyValues)


class CHValueRange(BaseRecordHandler):

    def parseBytes (self):
        minVal = globals.getDouble(self.readBytes(8))
        maxVal = globals.getDouble(self.readBytes(8))
        majorStep = globals.getDouble(self.readBytes(8))
        minorStep = globals.getDouble(self.readBytes(8))
        cross = globals.getDouble(self.readBytes(8))
        flags = globals.getSignedInt(self.readBytes(2))

        autoMin   = (flags & 0x0001)
        autoMax   = (flags & 0x0002)
        autoMajor = (flags & 0x0004)
        autoMinor = (flags & 0x0008)
        autoCross = (flags & 0x0010)
        logScale  = (flags & 0x0020)
        reversed  = (flags & 0x0040)
        maxCross  = (flags & 0x0080)
        bit8      = (flags & 0x0100)

        self.appendLine("min: %g (auto min: %s)"%(minVal, self.getYesNo(autoMin)))
        self.appendLine("max: %g (auto max: %s)"%(maxVal, self.getYesNo(autoMax)))
        self.appendLine("major step: %g (auto major: %s)"%
            (majorStep, self.getYesNo(autoMajor)))
        self.appendLine("minor step: %g (auto minor: %s)"%
            (minorStep, self.getYesNo(autoMinor)))
        self.appendLine("cross: %g (auto cross: %s) (max cross: %s)"%
            (cross, self.getYesNo(autoCross), self.getYesNo(maxCross)))
        self.appendLine("biff5 or above: %s"%self.getYesNo(bit8))


class CHBar(BaseRecordHandler):

    def parseBytes (self):
        overlap = globals.getSignedInt(self.readBytes(2))
        gap     = globals.getSignedInt(self.readBytes(2))
        flags   = globals.getUnsignedInt(self.readBytes(2))

        horizontal = (flags & 0x0001)
        stacked    = (flags & 0x0002)
        percent    = (flags & 0x0004)
        shadow     = (flags & 0x0008)

        self.appendLine("overlap width: %d"%overlap)
        self.appendLine("gap: %d"%gap)
        self.appendLine("horizontal: %s"%self.getYesNo(horizontal))
        self.appendLine("stacked: %s"%self.getYesNo(stacked))
        self.appendLine("percent: %s"%self.getYesNo(percent))
        self.appendLine("shadow: %s"%self.getYesNo(shadow))


class CHLine(BaseRecordHandler):

    def parseBytes (self):
        flags   = globals.getUnsignedInt(self.readBytes(2))
        stacked = (flags & 0x0001)
        percent = (flags & 0x0002)
        shadow  = (flags & 0x0004)

        self.appendLine("stacked: %s"%self.getYesNo(stacked))
        self.appendLine("percent: %s"%self.getYesNo(percent))
        self.appendLine("shadow: %s"%self.getYesNo(shadow))

