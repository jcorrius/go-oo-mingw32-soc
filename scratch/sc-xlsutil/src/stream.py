
import sys
import ole

recData = {
    0x000A: ["EOF", "End of File"],
    0x000C: ["CALCCOUNT", "Iteration Count"],
    0x000D: ["CALCMODE", "Calculation Mode"],
    0x000E: ["PRECISION", "Precision"],
    0x000F: ["REFMODE", "Reference Mode"],
    0x0100: ["SXVDEX", "Extended PivotTable View Fields"],
    0x0103: ["SXFORMULA", "PivotTable Formula Record"],
    0x0010: ["DELTA", "Iteration Increment"],
    0x0011: ["ITERATION", "Iteration Mode"],
    0x0122: ["SXDBEX", "PivotTable Cache Data"],
    0x0012: ["PROTECT", "Protection Flag"],
    0x013D: ["TABID", "Sheet Tab Index Array"],
    0x0013: ["PASSWORD", "Protection Password"],
    0x0014: ["HEADER", "Print Header on Each Page"],
    0x0015: ["FOOTER", "Print Footer on Each Page"],
    0x0160: ["USESELFS", "Natural Language Formulas Flag"],
    0x0161: ["DSF", "Double Stream File"],
    0x0016: ["EXTERNCOUNT", "Number of External References"],
    0x0017: ["EXTERNSHEET", "External Reference"],
    0x0019: ["WINDOWPROTECT", "Windows Are Protected"],
    0x01A9: ["USERBVIEW", "Workbook Custom View Settings"],
    0x01AA: ["USERSVIEWBEGIN", "Custom View Settings"],
    0x01AB: ["USERSVIEWEND", "End of Custom View Records"],
    0x01AD: ["QSI", "External Data Range"],
    0x01AE: ["SUPBOOK", "Supporting Workbook"],
    0x001A: ["VERTICALPAGEBREAKS", "Explicit Column Page Breaks"],
    0x01B0: ["CONDFMT", "Conditional Formatting Range Information"],
    0x01B1: ["CF", "Conditional Formatting Conditions"],
    0x01B2: ["DVAL", "Data Validation Information"],
    0x01B5: ["DCONBIN", "Data Consolidation Information"],
    0x01B6: ["TXO", "Text Object"],
    0x01B7: ["REFRESHALL", "Refresh Flag"],
    0x01B8: ["HLINK", "Hyperlink"],
    0x01BB: ["SXFDBTYPE", "SQL Datatype Identifier"],
    0x01BE: ["DV", "Data Validation Criteria"],
    0x001B: ["HORIZONTALPAGEBREAKS", "Explicit Row Page Breaks"],
    0x001C: ["NOTE", "Comment Associated with a Cell"],
    0x001D: ["SELECTION", "Current Selection"],
    0x0200: ["DIMENSIONS", "Cell Table Size"],
    0x0201: ["BLANK", "Cell Value"],
    0x0203: ["NUMBER", "Cell Value"],
    0x0204: ["LABEL", "Cell Value"],
    0x0205: ["BOOLERR", "Cell Value"],
    0x0207: ["STRING", "String Value of a Formula"],
    0x0208: ["ROW", "Describes a Row"],
    0x020B: ["INDEX", "Index Record"],
    0x0218: ["NAME", "Defined Name"],
    0x0221: ["ARRAY", "Array-Entered Formula"],
    0x0223: ["EXTERNNAME", "Externally Referenced Name"],
    0x0225: ["DEFAULTROWHEIGHT", "Default Row Height"],
    0x0231: ["FONT", "Font Description"],
    0x0236: ["TABLE", "Data Table"],
    0x0026: ["LEFTMARGIN", "Left Margin Measurement"],
    0x0027: ["RIGHTMARGIN", "Right Margin Measurement"],
    0x0028: ["TOPMARGIN", "Top Margin Measurement"],
    0x0293: ["STYLE", "Style Information"],
    0x0029: ["BOTTOMMARGIN", "Bottom Margin Measurement"],
    0x002A: ["PRINTHEADERS", "Print Row/Column Labels"],
    0x002B: ["PRINTGRIDLINES", "Print Gridlines Flag"],
    0x002F: ["FILEPASS", "File Is Password-Protected"],
    0x003C: ["CONTINUE", "Continues Long Records"],
    0x0406: ["FORMULA", "Cell Formula"],
    0x0040: ["BACKUP", "Save Backup Version of the File"],
    0x041E: ["FORMAT", "Number Format"],
    0x0041: ["PANE", "Number of Panes and Their Position"],
    0x0042: ["CODEPAGE/CODENAME", "Default Code Page/VBE Object Name"],
    0x004D: ["PLS", "Environment-Specific Print Record"],
    0x0050: ["DCON", "Data Consolidation Information"],
    0x0051: ["DCONREF", "Data Consolidation References"],
    0x0052: ["DCONNAME", "Data Consolidation Named References"],
    0x0055: ["DEFCOLWIDTH", "Default Width for Columns"],
    0x0059: ["XCT", "CRN Record Count"],
    0x005A: ["CRN", "Nonresident Operands"],
    0x005B: ["FILESHARING", "File-Sharing Information"],
    0x005C: ["WRITEACCESS", "Write Access User Name"],
    0x005D: ["OBJ", "Describes a Graphic Object"],
    0x005E: ["UNCALCED", "Recalculation Status"],
    0x005F: ["SAVERECALC", "Recalculate Before Save"],
    0x0060: ["TEMPLATE", "Workbook Is a Template"],
    0x0063: ["OBJPROTECT", "Objects Are Protected"],
    0x007D: ["COLINFO", "Column Formatting Information"],
    0x007E: ["RK", "Cell Value"],
    0x007F: ["IMDATA", "Image Data"],
    0x0809: ["BOF", "Beginning of File"],
    0x0080: ["GUTS", "Size of Row and Column Gutters"],
    0x0081: ["WSBOOL", "Additional Workspace Information"],
    0x0082: ["GRIDSET", "State Change of Gridlines Option"],
    0x0083: ["HCENTER", "Center Between Horizontal Margins"],
    0x0084: ["VCENTER", "Center Between Vertical Margins"],
    0x0085: ["BOUNDSHEET", "Sheet Information"],
    0x0086: ["WRITEPROT", "Workbook Is Write-Protected"],
    0x0087: ["ADDIN", "Workbook Is an Add-in Macro"],
    0x0088: ["EDG", "Edition Globals"],
    0x0089: ["PUB", "Publisher"],
    0x008C: ["COUNTRY", "Default Country and WIN.INI Country"],
    0x008D: ["HIDEOBJ", "Object Display Options"],
    0x0090: ["SORT", "Sorting Options"],
    0x0091: ["SUB", "Subscriber"],
    0x0092: ["PALETTE", "Color Palette Definition"],
    0x0094: ["LHRECORD", ".WK? File Conversion Information"],
    0x0095: ["LHNGRAPH", "Named Graph Information"],
    0x0096: ["SOUND", "Sound Note"],
    0x0098: ["LPR", "Sheet Was Printed Using LINE.PRINT()"],
    0x0099: ["STANDARDWIDTH", "Standard Column Width"],
    0x009A: ["FNGROUPNAME", "Function Group Name"],
    0x009B: ["FILTERMODE", "Sheet Contains Filtered List"],
    0x009C: ["FNGROUPCOUNT", "Built-in Function Group Count"],
    0x009D: ["AUTOFILTERINFO", "Drop-Down Arrow Count"],
    0x009E: ["AUTOFILTER", "AutoFilter Data"],
    0x00A0: ["SCL", "Window Zoom Magnification"],
    0x00A1: ["SETUP", "Page Setup"],
    0x00A9: ["COORDLIST", "Polygon Object Vertex Coordinates"],
    0x00AB: ["GCW", "Global Column-Width Flags"],
    0x00AE: ["SCENMAN", "Scenario Output Data"],
    0x00AF: ["SCENARIO", "Scenario Data"],
    0x00B0: ["SXVIEW", "View Definition"],
    0x00B1: ["SXVD", "View Fields"],
    0x00B2: ["SXVI", "View Item"],
    0x00B4: ["SXIVD", "Row/Column Field IDs"],
    0x00B5: ["SXLI", "Line Item Array"],
    0x00B6: ["SXPI", "Page Item"],
    0x00B8: ["DOCROUTE", "Routing Slip Information"],
    0x00B9: ["RECIPNAME", "Recipient Name"],
    0x00BC: ["SHRFMLA", "Shared Formula"],
    0x00BD: ["MULRK", "Multiple RK Cells"],
    0x00BE: ["MULBLANK", "Multiple Blank Cells"],
    0x00C1: ["MMS", "ADDMENU/DELMENU Record Group Count"],
    0x00C2: ["ADDMENU", "Menu Addition"],
    0x00C3: ["DELMENU", "Menu Deletion"],
    0x00C5: ["SXDI", "Data Item"],
    0x00C6: ["SXDB", "PivotTable Cache Data"],
    0x00CD: ["SXSTRING", "String"],
    0x00D0: ["SXTBL", "Multiple Consolidation Source Info"],
    0x00D1: ["SXTBRGIITM", "Page Item Name Count"],
    0x00D2: ["SXTBPG", "Page Item Indexes"],
    0x00D3: ["OBPROJ", "Visual Basic Project"],
    0x00D5: ["SXIDSTM", "Stream ID"],
    0x00D6: ["RSTRING", "Cell with Character Formatting"],
    0x00D7: ["DBCELL", "Stream Offsets"],
    0x00DA: ["BOOKBOOL", "Workbook Option Flag"],
    0x00DC: ["PARAMQRY", "Query Parameters"],
    0x00DC: ["SXEXT", "External Source Information"],
    0x00DD: ["SCENPROTECT", "Scenario Protection"],
    0x00DE: ["OLESIZE", "Size of OLE Object"],
    0x00DF: ["UDDESC", "Description String for Chart Autoformat"],
    0x00E0: ["XF", "Extended Format"],
    0x00E1: ["INTERFACEHDR", "Beginning of User Interface Records"],
    0x00E2: ["INTERFACEEND", "End of User Interface Records"],
    0x00E3: ["SXVS", "View Source"],
    0x00EA: ["TABIDCONF", "Sheet Tab ID of Conflict History"],
    0x00EB: ["MSODRAWINGGROUP", "Microsoft Office Drawing Group"],
    0x00EC: ["MSODRAWING", "Microsoft Office Drawing"],
    0x00ED: ["MSODRAWINGSELECTION", "Microsoft Office Drawing Selection"],
    0x00F0: ["SXRULE", "PivotTable Rule Data"],
    0x00F1: ["SXEX", "PivotTable View Extended Information"],
    0x00F2: ["SXFILT", "PivotTable Rule Filter"],
    0x00F6: ["SXNAME", "PivotTable Name"],
    0x00F7: ["SXSELECT", "PivotTable Selection Information"],
    0x00F8: ["SXPAIR", "PivotTable Name Pair"],
    0x00F9: ["SXFMLA", "PivotTable Parsed Expression"],
    0x00FB: ["SXFORMAT", "PivotTable Format Record"],
    0x00FC: ["SST", "Shared String Table"],
    0x00FD: ["LABELSST", "Cell Value"],
    0x003D: ["WINDOW1", "Window Information"],
    0x0022: ["DATEMODE", "Base Date for Displaying Date Values"],
    0x023E: ["WINDOW2", "Sheet Window Information"],
    0x01AF: ["PROT4REV", "Shared Workbook Protection Flag"],
    0x01BC: ["PROT4REVPASS", "Shared Workbook Protection Password"],
    0x0031: ["FONT", "Font and Character Formatting"],
    0x00FF: ["EXTSST", "Extended Shared String Table"] }

def output (msg):
    sys.stdout.write(msg)

class XLStream(object):

    def __init__ (self, file):
        self.chars = file.read()
        self.size = len(self.chars)
        self.pos = 0
        self.version = None

        self.header = None
        self.MSAT = None

    def __printSep (self, c='-', w=68, prefix=''):
        print(prefix + c*w)

    def printStreamInfo (self):
        self.__printSep('=', 68)
        print("Excel File Format Dumper by Kohei Yoshida")
        print("  total stream size: %d bytes"%self.size)
        self.__printSep('=', 68)
        print('')

    def printHeader (self):
        self.header = ole.Header(self.chars)
        self.pos = self.header.parse()
        self.header.output()
        self.MSAT = self.header.getMSAT()

    def printMSAT (self):
        self.MSAT.output()

    def dumpHeader (self):
        oleobj = ole.Header(self.chars)
        self.pos = oleobj.dumpBytes()

    def seekBOF (self):
        while self.pos < self.size-1:
            b1, b2 = ord(self.chars[self.pos]), ord(self.chars[self.pos+1])
            word = b1 + b2*256
            if word == 0x0809:
                self.version = 'BIFF5/BIFF8'
                return
            self.pos += 2

    def readRaw (self, size=1):
        # assume little endian
        bytes = 0
        for i in xrange(0, size):
            b = ord(self.chars[self.pos])
            if i == 0:
                bytes = b
            else:
                bytes += b*(256**i)
            self.pos += 1

        return bytes

    def readByteArray (self, size=1):
        bytes = []
        for i in xrange(0, size):
            bytes.append(ord(self.chars[self.pos]))
            self.pos += 1
        return bytes

    def readRecord (self):
        pos = self.pos
        header = self.readRaw(2)
        size = self.readRaw(2)
        print("")
        self.__printSep('=', 61, "%4.4Xh: "%header)
        if recData.has_key(header):
            print("%4.4Xh: %s - %s (%4.4Xh)"%(header, recData[header][0], recData[header][1], header))
        else:
            print("%4.4Xh: [unknown record name] (%4.4Xh)"%(header, header))
        print("%4.4Xh:   size = %d; pos = %d"%(header, size, pos))
        self.__printSep('-', 61, "%4.4Xh: "%header)
        bytes = self.readByteArray(size)
        for i in xrange(0, size):
            if (i+1) % 16 == 1:
                output("%4.4Xh: "%header)
            output("%2.2X "%bytes[i])
            if (i+1) % 16 == 0 and i != size-1:
                print("")
        if size > 0:
            print("")

        return header
