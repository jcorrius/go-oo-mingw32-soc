
import struct
import globals

# -------------------------------------------------------------------
# record handler classes

class BaseRecordHandler(object):

    def __init__ (self, recordType, recordInstance, size, bytes, prefix=''):
        self.recordType = recordType
        self.recordInstance = recordInstance
        self.size = size
        self.bytes = bytes
        self.lines = []
        self.prefix = prefix
        self.pos = 0       # current byte position

    def parseBytes (self):
        """Parse the original bytes and generate human readable output.

The derived class should only worry about overwriting this function.  The
bytes are given as self.bytes, and call self.appendLine([new line]) to
append a line to be displayed.
"""
        pass

    def __print (self, text):
        print(self.prefix + text)

    def output (self):
        self.parseBytes()
        self.__print("%4.4Xh: %s"%(self.recordType, "-"*61))
        for line in self.lines:
            self.__print("%4.4Xh: %s"%(self.recordType, line))

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

# -------------------------------------------------------------------
# special record handler: properties

class Property(BaseRecordHandler):
    """Shape properties."""

    def parseBytes (self):
        # each prop entry takes 6 bytes; complex stuff comes after
        # prop entries and fills remaining record space
        complexBytes = self.bytes[self.pos+self.recordInstance*6:]

        # recordInstance gives number of properties
        for i in xrange(0, self.recordInstance):
            propType = self.readUnsignedInt(2)
            propValue = self.readUnsignedInt(4)

            isComplex = (propType & 0x8000) != 0
            isBlip = ((propType & 0x4000) != 0) and not isComplex
            propType = (propType & 0x3FFF)

            if propData.has_key(propType):
                handler = propData[propType][1](propType, propValue, isComplex, isBlip, self.appendLine)
                handler.output()
            else:
                self.appendLine("%4.4Xh: [unknown property type: %4.4Xh, value: %8.8Xh, complex: %d, blip: %d]"%(propType, propValue, isComplex, isBlip))

# -------------------------------------------------------------------
# special record handlers: text style properties

class TextStyles(BaseRecordHandler):
    """Text style properties."""

    def parseBytes (self):
        # 4 bytes: total len of para attribs
        # <para attribs>
        # 4 bytes: total len of char attribs
        # <char attribs>
        paraAttribLen = self.readUnsignedInt(4)
        paraAttribEndPos = self.pos + paraAttribLen
        while self.pos < paraAttribEndPos:
            self.parseParaStyle()
            self.appendLine("-"*61)

        charAttribLen = self.readUnsignedInt(4)
        charAttribEndPos = self.pos + charAttribLen
        while self.pos < charAttribEndPos:
            self.parseCharStyle()
            self.appendLine("-"*61)

    def appendParaProp (self, text):
        self.appendLine("para prop given: "+text)

    def appendCharProp (self, text):
        self.appendLine("char prop given: "+text)

    def parseParaStyle (self):
        indentLevel = self.readUnsignedInt(2)
        styleMask = self.readUnsignedInt(4)

        self.appendLine("para props for indent: %d"%indentLevel)

        if styleMask & 0x000F:
            bulletFlags = self.readUnsignedInt(2)
            # filter bits not in flag field
            bulletFlags = bulletFlags & (styleMask & 0x000F)
            self.appendParaProp("bullet flags %4.4Xh"%bulletFlags)

        if styleMask & 0x0080:
            bulletChar = self.readUnsignedInt(2)
            self.appendParaProp("bullet char %4.4Xh"%bulletChar)

        if styleMask & 0x0010:
            bulletTypeface = self.readUnsignedInt(2)
            self.appendParaProp("bullet typeface %d"%bulletTypeface)

        if styleMask & 0x0040:
            bulletSize = self.readSignedInt(2)
            self.appendParaProp("bullet size %d"%bulletSize)

        if styleMask & 0x0020:
            bulletColorAtom = ColorPropertyHandler(self.readUnsignedInt(2), self.readUnsignedInt(4), False, False, self.appendParaProp)
            bulletColorAtom.output()
            self.appendParaProp("bullet color atom")

        if styleMask & 0x0800:
            paraAlignment = self.readSignedInt(2)
            self.appendParaProp("para alignment %4.4Xh"%paraAlignment)

        if styleMask & 0x0400:
            paraIndent = self.readSignedInt(2)
            self.appendParaProp("para indent %d"%paraIndent)

        if styleMask & 0x0200:
            unused = self.readSignedInt(2)
            self.appendParaProp("unused para property %4.4Xh"%unused)

        if styleMask & 0x0100:
            paraLeftMargin = self.readSignedInt(2)
            self.appendParaProp("para left margin %d"%paraLeftMargin)

        if styleMask & 0x1000:
            paraLineSpacing = self.readSignedInt(2)
            self.appendParaProp("para linespacing %d"%paraLineSpacing)

        if styleMask & 0x2000:
            paraSpaceBefore = self.readSignedInt(2)
            self.appendParaProp("para space before %d"%paraSpaceBefore)

        if styleMask & 0x4000:
            paraSpaceAfter = self.readSignedInt(2)
            self.appendParaProp("para space after %d"%paraSpaceAfter)

        if styleMask & 0x8000:
            paraDefaultTabSize = self.readSignedInt(2)
            self.appendParaProp("para default tab size %d"%paraDefaultTabSize)

        if styleMask & 0x100000:
            numTabStops = self.readUnsignedInt(2)
            for i in xrange(0, numTabStops):
                tabDistance = self.readUnsignedInt(2)
                tabAlignment = self.readUnsignedInt(2)
                self.appendParaProp("para tab stop %d: distance %d, align %4.4Xh"%(i, tabDistance, tabAlignment))

        if styleMask & 0x10000:
            paraBaseline = self.readUnsignedInt(2)
            self.appendParaProp("para baseline %d"%paraBaseline)

        if styleMask & 0xE0000:
            paraAsianLinebreaking = self.readUnsignedInt(2)
            # filter bits not in flag field
            paraAsianLinebreaking = paraAsianLinebreaking & ((styleMask & 0xE0000) / 0x20000)
            self.appendParaProp("para asian line breaking flags %4.4Xh"%paraAsianLinebreaking)

        if styleMask & 0x200000:
            paraTextDirection = self.readUnsignedInt(2)
            self.appendParaProp("para text direction %4.4Xh"%paraTextDirection)

    def parseCharStyle (self):
        styleMask = self.readUnsignedInt(4)

        if styleMask & 0xFFFF:
            charFlags = self.readUnsignedInt(2)
            self.appendCharProp("char flags %4.4Xh"%charFlags)

        if styleMask & 0x10000:
            typeFace = self.readUnsignedInt(2)
            self.appendCharProp("char typeface %d"%typeFace)

        if styleMask & 0x200000:
            oldTypeFace = self.readUnsignedInt(2)
            self.appendCharProp("char old asian typeface %d"%oldTypeFace)

        if styleMask & 0x400000:
            ansiTypeFace = self.readUnsignedInt(2)
            self.appendCharProp("char ansi typeface %d"%ansiTypeFace)

        if styleMask & 0x800000:
            symbolTypeFace = self.readUnsignedInt(2)
            self.appendCharProp("char symbol typeface %d"%symbolTypeFace)

        if styleMask & 0x20000:
            fontSize = self.readUnsignedInt(2)
            self.appendCharProp("char font size %d"%fontSize)

        if styleMask & 0x40000:
            charColorAtom = ColorPropertyHandler(self.readUnsignedInt(2), self.readUnsignedInt(4), False, False, self.appendCharProp)
            charColorAtom.output()
            self.appendCharProp("char color atom")

        if styleMask & 0x80000:
            fontPosition = self.readSignedInt(2)
            self.appendCharProp("char font position %d"%fontPosition)


# -------------------------------------------------------------------
# special record handlers: property atoms

class BasePropertyHandler():
    """Base property handler."""

    def __init__ (self, propType, propValue, isComplex, isBlip, printer):
        self.propType = propType
        self.propValue = propValue
        self.isComplex = isComplex
        self.isBlip = isBlip
        self.printer = printer
    
    def output (self):
        if propData.has_key(self.propType):
            propEntry = propData[self.propType]
            self.printer("%4.4Xh: %s = %8.8Xh [\"%s\" - default handler]"%(self.propType, propEntry[0], self.propValue, propEntry[2]))

class BoolPropertyHandler(BasePropertyHandler):
    """Bool properties."""

    def output (self):
        bitMask = 1
        for i in xrange(self.propType, self.propType-32):
            if propData.has_key(i):
                propEntry = propData[i]
                if type(propEntry[1]) == type(BoolPropertyHandler):
                    flagValue = self.getTrueFalse(self.propValue & bitMask)
                    self.printer("%4.4Xh: %s = %d [\"%s\"]"%(self.propType, propEntry[0], flagValue, propEntry[2]))
            bitMask *= 2

            
class LongPropertyHandler(BasePropertyHandler):
    """Long property."""
        
class MsoArrayPropertyHandler(BasePropertyHandler):
    """MsoArray property."""

class UniCharPropertyHandler(BasePropertyHandler):
    """unicode string property."""  

class FixedPointHandler(BasePropertyHandler):
    """FixedPoint property."""
    
class ColorPropertyHandler(BasePropertyHandler):
    """Color property."""   

    def split (self, packedColor):
        return (packedColor & 0xFF0000) / 0x10000, (packedColor & 0xFF00) / 0x100, (packedColor & 0xFF)
    
    def output (self):
        propEntry = ["<color atom>", None, "undocumented color property"]
        if propData.has_key(self.propType):
            propEntry = propData[self.propType]
        colorValue = self.propValue & 0xFFFFFF
        if self.propValue & 0xFE000000 == 0xFE000000:
            self.printer("%4.4Xh: %s = (%d,%d,%d) [\"%s\"]"%(self.propType, propEntry[0], split(colorValue), propEntry[2]))
        elif self.propValue & 0x08000000 or self.propValue & 0x10000000:
            self.printer("%4.4Xh: %s = schemecolor(%d) [\"%s\"]"%(self.propType, propEntry[0], colorValue, propEntry[2]))
        elif self.propValue & 0x04000000:
            self.printer("%4.4Xh: %s = colorschemecolor(%d) [\"%s\"]"%(self.propType, propEntry[0], colorValue, propEntry[2]))
        else:
            self.printer("%4.4Xh: %s = <unidentified color>(%4.4Xh) [\"%s\"]"%(self.propType, propEntry[0], colorValue, propEntry[2]))


class CharPropertyHandler(BasePropertyHandler):
    """string property."""  

class HandlesPropertyHandler(BasePropertyHandler):
    """string property."""  

# -------------------------------------------------------------------
# special record handler: properties

    # opcode: [canonical name, prop handler, comment]

propData = {

   4:  ["DFF_Prop_Rotation",                     FixedPointHandler,        "degrees"],
 119:  ["DFF_Prop_LockRotation",                 BoolPropertyHandler,              "No rotation"],
 120:  ["DFF_Prop_LockAspectRatio",              BoolPropertyHandler,              "Don't allow changes in aspect ratio"],
 121:  ["DFF_Prop_LockPosition",                 BoolPropertyHandler,              "Don't allow the shape to be moved"],
 122:  ["DFF_Prop_LockAgainstSelect",            BoolPropertyHandler,              "Shape may not be selected"],
 123:  ["DFF_Prop_LockCropping",                 BoolPropertyHandler,              "No cropping this shape"],
 124:  ["DFF_Prop_LockVertices",                 BoolPropertyHandler,              "Edit Points not allowed"],
 125:  ["DFF_Prop_LockText",                     BoolPropertyHandler,              "Do not edit text"],
 126:  ["DFF_Prop_LockAdjustHandles",            BoolPropertyHandler,              "Do not adjust"],
 127:  ["DFF_Prop_LockAgainstGrouping",          BoolPropertyHandler,              "Do not group this shape"],
      
 128:  ["DFF_Prop_lTxid",                        LongPropertyHandler,              "id for the text, value determined by the host"],
 129:  ["DFF_Prop_dxTextLeft",                   LongPropertyHandler,              "margins relative to shape's inscribed text rectangle (in EMUs)"],
 130:  ["DFF_Prop_dyTextTop",                    LongPropertyHandler, ""],
 131:  ["DFF_Prop_dxTextRight",                  LongPropertyHandler, ""],
 132:  ["DFF_Prop_dyTextBottom",                 LongPropertyHandler, ""],
 133:  ["DFF_Prop_WrapText",                     LongPropertyHandler,              "Wrap text at shape margins"],
 134:  ["DFF_Prop_scaleText",                    LongPropertyHandler,              "Text zoom/scale (used if fFitTextToShape)"],
 135:  ["DFF_Prop_anchorText",                   LongPropertyHandler,              "How to anchor the text"],
 136:  ["DFF_Prop_txflTextFlow",                 LongPropertyHandler,              "Text flow"],
 137:  ["DFF_Prop_cdirFont",                     LongPropertyHandler,              "Font rotation in 90 degree steps"],
 138:  ["DFF_Prop_hspNext",                      LongPropertyHandler,              "ID of the next shape (used by Word for linked textboxes)"],
 139:  ["DFF_Prop_txdir",                        LongPropertyHandler,              "Bi-Di Text direction"],
 187:  ["DFF_Prop_SelectText",                   BoolPropertyHandler,              "TRUE if single click selects text, FALSE if two clicks"],
 188:  ["DFF_Prop_AutoTextMargin",               BoolPropertyHandler,              "use host's margin calculations"],
 189:  ["DFF_Prop_RotateText",                   BoolPropertyHandler,              "Rotate text with shape"],
 190:  ["DFF_Prop_FitShapeToText",               BoolPropertyHandler,              "Size shape to fit text size"],
 191:  ["DFF_Prop_FitTextToShape",               BoolPropertyHandler,              "Size text to fit shape size"],
      
 192:  ["DFF_Prop_gtextUNICODE",                 UniCharPropertyHandler,            "UNICODE text string"],
 193:  ["DFF_Prop_gtextRTF",                     CharPropertyHandler,             "RTF text string"],
 194:  ["DFF_Prop_gtextAlign",                   LongPropertyHandler,              "alignment on curve"],
 195:  ["DFF_Prop_gtextSize",                    LongPropertyHandler,              "default point size"],
 196:  ["DFF_Prop_gtextSpacing",                 LongPropertyHandler,              "fixed point 16.16"],
 197:  ["DFF_Prop_gtextFont",                    UniCharPropertyHandler,            "font family name"],
 240:  ["DFF_Prop_gtextFReverseRows",            BoolPropertyHandler,              "Reverse row order"],
 241:  ["DFF_Prop_fGtext",                       BoolPropertyHandler,              "Has text effect"],
 242:  ["DFF_Prop_gtextFVertical",               BoolPropertyHandler,              "Rotate characters"],
 243:  ["DFF_Prop_gtextFKern",                   BoolPropertyHandler,              "Kern characters"],
 244:  ["DFF_Prop_gtextFTight",                  BoolPropertyHandler,              "Tightening or tracking"],
 245:  ["DFF_Prop_gtextFStretch",                BoolPropertyHandler,              "Stretch to fit shape"],
 246:  ["DFF_Prop_gtextFShrinkFit",              BoolPropertyHandler,              "Char bounding box"],
 247:  ["DFF_Prop_gtextFBestFit",                BoolPropertyHandler,              "Scale text-on-path"],
 248:  ["DFF_Prop_gtextFNormalize",              BoolPropertyHandler,              "Stretch char height"],
 249:  ["DFF_Prop_gtextFDxMeasure",              BoolPropertyHandler,              "Do not measure along path"],
 250:  ["DFF_Prop_gtextFBold",                   BoolPropertyHandler,              "Bold font"],
 251:  ["DFF_Prop_gtextFItalic",                 BoolPropertyHandler,              "Italic font"],
 252:  ["DFF_Prop_gtextFUnderline",              BoolPropertyHandler,              "Underline font"],
 253:  ["DFF_Prop_gtextFShadow",                 BoolPropertyHandler,              "Shadow font"],
 254:  ["DFF_Prop_gtextFSmallcaps",              BoolPropertyHandler,              "Small caps font"],
 255:  ["DFF_Prop_gtextFStrikethrough",          BoolPropertyHandler,              "Strike through font"],
      
 256:  ["DFF_Prop_cropFromTop",                  FixedPointHandler,        "Fraction times total image height, as appropriate."],
 257:  ["DFF_Prop_cropFromBottom",               FixedPointHandler,        "Fraction times total image height, as appropriate."],
 258:  ["DFF_Prop_cropFromLeft",                 FixedPointHandler,        "Fraction times total image width, as appropriate."],
 259:  ["DFF_Prop_cropFromRight",                FixedPointHandler,        "Fraction times total image width, as appropriate."],
 260:  ["DFF_Prop_pib",                          LongPropertyHandler,              "Blip ID to display"],
 261:  ["DFF_Prop_pibName",                      UniCharPropertyHandler,            "Blip file name"],
 262:  ["DFF_Prop_pibFlags",                     LongPropertyHandler,              "Blip flags"],
 263:  ["DFF_Prop_pictureTransparent",           LongPropertyHandler,              "transparent color (none if ~0UL)"],
 264:  ["DFF_Prop_pictureContrast",              LongPropertyHandler,              "contrast setting"],
 265:  ["DFF_Prop_pictureBrightness",            LongPropertyHandler,              "brightness setting"],
 266:  ["DFF_Prop_pictureGamma",                 FixedPointHandler,        "gamma"],
 267:  ["DFF_Prop_pictureId",                    LongPropertyHandler,              "Host-defined ID for OLE objects (usually a pointer)"],
 268:  ["DFF_Prop_pictureDblCrMod",              LongPropertyHandler,              "MSO_CLR Modification used if shape has double shadow"],
 269:  ["DFF_Prop_pictureFillCrMod",             LongPropertyHandler, ""],
 270:  ["DFF_Prop_pictureLineCrMod",             LongPropertyHandler, ""],
 271:  ["DFF_Prop_pibPrint",                     LongPropertyHandler,              "Blip ID to display when printing"],
 272:  ["DFF_Prop_pibPrintName",                 UniCharPropertyHandler,            "Blip file name"],
 273:  ["DFF_Prop_pibPrintFlags",                LongPropertyHandler,              "Blip flags"],
 316:  ["DFF_Prop_fNoHitTestPicture",            BoolPropertyHandler,              "Do not hit test the picture"],
 317:  ["DFF_Prop_pictureGray",                  BoolPropertyHandler,              "grayscale display"],
 318:  ["DFF_Prop_pictureBiLevel",               BoolPropertyHandler,              "bi-level display"],
 319:  ["DFF_Prop_pictureActive",                BoolPropertyHandler,              "Server is active (OLE objects only)"],
      
 320:  ["DFF_Prop_geoLeft",                      LongPropertyHandler,              "Defines the G (geometry) coordinate space."],
 321:  ["DFF_Prop_geoTop",                       LongPropertyHandler, ""],
 322:  ["DFF_Prop_geoRight",                     LongPropertyHandler, ""],
 323:  ["DFF_Prop_geoBottom",                    LongPropertyHandler, ""],
 324:  ["DFF_Prop_shapePath",                    LongPropertyHandler,              "MSO_SHAPEPATH, todo"],
 325:  ["DFF_Prop_pVertices",                    MsoArrayPropertyHandler,         "An array of points, in G units."],
 326:  ["DFF_Prop_pSegmentInfo",                 MsoArrayPropertyHandler, ""],
 327:  ["DFF_Prop_adjustValue",                  LongPropertyHandler,              "Adjustment values corresponding to the positions of the"],
 328:  ["DFF_Prop_adjust2Value",                 LongPropertyHandler,              "adjust handles of the shape. The number of values used"],
 329:  ["DFF_Prop_adjust3Value",                 LongPropertyHandler,              "and their allowable ranges vary from shape type to shape type."],
 330:  ["DFF_Prop_adjust4Value",                 LongPropertyHandler, ""],
 331:  ["DFF_Prop_adjust5Value",                 LongPropertyHandler, ""],
 332:  ["DFF_Prop_adjust6Value",                 LongPropertyHandler, ""],
 333:  ["DFF_Prop_adjust7Value",                 LongPropertyHandler, ""],
 334:  ["DFF_Prop_adjust8Value",                 LongPropertyHandler, ""],
 335:  ["DFF_Prop_adjust9Value",                 LongPropertyHandler, ""],
 336:  ["DFF_Prop_adjust10Value",                LongPropertyHandler, ""],
 337:  ["DFF_Prop_connectorPoints",             MsoArrayPropertyHandler, ""],
 339:  ["DFF_Prop_stretchPointX",               LongPropertyHandler, ""],
 340:  ["DFF_Prop_stretchPointY",               LongPropertyHandler, ""],
 341:  ["DFF_Prop_Handles",                     HandlesPropertyHandler, ""],
 342:  ["DFF_Prop_pFormulas",                   LongPropertyHandler, ""],
 343:  ["DFF_Prop_textRectangles",              LongPropertyHandler, ""],
 344:  ["DFF_Prop_connectorType",               LongPropertyHandler,               "0=none, 1=segments, 2=custom, 3=rect"],
 378:  ["DFF_Prop_fShadowOK",                    BoolPropertyHandler,              "Shadow may be set"],
 379:  ["DFF_Prop_f3DOK",                        BoolPropertyHandler,              "3D may be set"],
 380:  ["DFF_Prop_fLineOK",                      BoolPropertyHandler,              "Line style may be set"],
 381:  ["DFF_Prop_fGtextOK",                     BoolPropertyHandler,              "Text effect (FontWork) supported"],
 382:  ["DFF_Prop_fFillShadeShapeOK",            BoolPropertyHandler, ""],
 383:  ["DFF_Prop_fFillOK",                      BoolPropertyHandler,              "OK to fill the shape through the UI or VBA?"],
      
 384:  ["DFF_Prop_fillType",                     LongPropertyHandler,              "MSO_FILLTYPE Type of fill"],
 385:  ["DFF_Prop_fillColor",                    ColorPropertyHandler,             "Foreground color"],
 386:  ["DFF_Prop_fillOpacity",                  FixedPointHandler,        "Fill Opacity"],
 387:  ["DFF_Prop_fillBackColor",                ColorPropertyHandler,             "Background color"],
 388:  ["DFF_Prop_fillBackOpacity",              FixedPointHandler,        "Shades only"],
 389:  ["DFF_Prop_fillCrMod",                    ColorPropertyHandler,             "Modification for BW views"],
 390:  ["DFF_Prop_fillBlip",                     LongPropertyHandler,              "Pattern/texture blip ID"],
 391:  ["DFF_Prop_fillBlipName",                 UniCharPropertyHandler,            "Blip file name"],
 392:  ["DFF_Prop_fillBlipFlags",                LongPropertyHandler,              "Blip flags"],
 393:  ["DFF_Prop_fillWidth",                    LongPropertyHandler,              "How big (A units) to make a metafile texture."],
 394:  ["DFF_Prop_fillHeight",                   LongPropertyHandler, ""],
 395:  ["DFF_Prop_fillAngle",                    LongPropertyHandler,              "Fade angle - degrees in 16.16"],
 396:  ["DFF_Prop_fillFocus",                    LongPropertyHandler,              "Linear shaded fill focus percent"],
 397:  ["DFF_Prop_fillToLeft",                   FixedPointHandler,        "Fraction 16.16"],
 398:  ["DFF_Prop_fillToTop",                    FixedPointHandler,        "Fraction 16.16"],
 399:  ["DFF_Prop_fillToRight",                  FixedPointHandler,        "Fraction 16.16"],
 400:  ["DFF_Prop_fillToBottom",                 FixedPointHandler,        "Fraction 16.16"],
 401:  ["DFF_Prop_fillRectLeft",                 LongPropertyHandler,              "For shaded fills, use the specified rectangle instead of the shape's bounding rect to define how large the fade is going to be."],
 402:  ["DFF_Prop_fillRectTop",                  LongPropertyHandler, ""],
 403:  ["DFF_Prop_fillRectRight",                LongPropertyHandler, ""],
 404:  ["DFF_Prop_fillRectBottom",               LongPropertyHandler, ""],
 405:  ["DFF_Prop_fillDztype",                   LongPropertyHandler, ""],
 406:  ["DFF_Prop_fillShadePreset",              LongPropertyHandler,              "Special shades"],
 407:  ["DFF_Prop_fillShadeColors",              MsoArrayPropertyHandler,         "a preset array of colors"],
 408:  ["DFF_Prop_fillOriginX",                  LongPropertyHandler, ""],
 409:  ["DFF_Prop_fillOriginY",                  LongPropertyHandler, ""],
 410:  ["DFF_Prop_fillShapeOriginX",             LongPropertyHandler, ""],
 411:  ["DFF_Prop_fillShapeOriginY",             LongPropertyHandler, ""],
 412:  ["DFF_Prop_fillShadeType",                LongPropertyHandler,              "Type of shading, if a shaded (gradient) fill."],
 443:  ["DFF_Prop_fFilled",                      BoolPropertyHandler,              "Is shape filled?"],
 444:  ["DFF_Prop_fHitTestFill",                 BoolPropertyHandler,              "Should we hit test fill?"],
 445:  ["DFF_Prop_fillShape",                    BoolPropertyHandler,              "Register pattern on shape"],
 446:  ["DFF_Prop_fillUseRect",                  BoolPropertyHandler,              "Use the large rect?"],
 447:  ["DFF_Prop_fNoFillHitTest",               BoolPropertyHandler,              "Hit test a shape as though filled"],
      
 448:  ["DFF_Prop_lineColor",                    ColorPropertyHandler,             "Color of line"],
 449:  ["DFF_Prop_lineOpacity",                  LongPropertyHandler,              "Not implemented"],
 450:  ["DFF_Prop_lineBackColor",                ColorPropertyHandler,             "Background color"],
 451:  ["DFF_Prop_lineCrMod",                    ColorPropertyHandler,             "Modification for BW views"],
 452:  ["DFF_Prop_lineType",                     LongPropertyHandler,              "Type of line"],
 453:  ["DFF_Prop_lineFillBlip",                 LongPropertyHandler,              "Pattern/texture blip ID"],
 454:  ["DFF_Prop_lineFillBlipName",             UniCharPropertyHandler,            "Blip file name"],
 455:  ["DFF_Prop_lineFillBlipFlags",            LongPropertyHandler,              "Blip flags"],
 456:  ["DFF_Prop_lineFillWidth",                LongPropertyHandler,              "How big (A units) to make a metafile texture."],
 457:  ["DFF_Prop_lineFillHeight",               LongPropertyHandler, ""],
 458:  ["DFF_Prop_lineFillDztype",               LongPropertyHandler,              "How to interpret fillWidth/Height numbers."],
 459:  ["DFF_Prop_lineWidth",                    LongPropertyHandler,              "A units; 1pt == 12700 EMUs"],
 460:  ["DFF_Prop_lineMiterLimit",               FixedPointHandler,        "ratio (16.16) of width"],
 461:  ["DFF_Prop_lineStyle",                    LongPropertyHandler,              "Draw parallel lines?"],
 462:  ["DFF_Prop_lineDashing",                  LongPropertyHandler,              "Can be overridden by:"],
 463:  ["DFF_Prop_lineDashStyle",                MsoArrayPropertyHandler,         "As Win32 ExtCreatePen"],
 464:  ["DFF_Prop_lineStartArrowhead",           LongPropertyHandler,              "Arrow at start"],
 465:  ["DFF_Prop_lineEndArrowhead",             LongPropertyHandler,              "Arrow at end"],
 466:  ["DFF_Prop_lineStartArrowWidth",          LongPropertyHandler,              "Arrow at start"],
 467:  ["DFF_Prop_lineStartArrowLength",         LongPropertyHandler,              "Arrow at end"],
 468:  ["DFF_Prop_lineEndArrowWidth",            LongPropertyHandler,              "Arrow at start"],
 469:  ["DFF_Prop_lineEndArrowLength",           LongPropertyHandler,              "Arrow at end"],
 470:  ["DFF_Prop_lineJoinStyle",                LongPropertyHandler,              "How to join lines"],
 471:  ["DFF_Prop_lineEndCapStyle",              LongPropertyHandler,              "How to end lines"],
 507:  ["DFF_Prop_fArrowheadsOK",                BoolPropertyHandler,              "Allow arrowheads if prop. is set"],
 508:  ["DFF_Prop_fLine",                        BoolPropertyHandler,              "Any line?"],
 509:  ["DFF_Prop_fHitTestLine",                 BoolPropertyHandler,              "Should we hit test lines?"],
 510:  ["DFF_Prop_lineFillShape",                BoolPropertyHandler,              "Register pattern on shape"],
 511:  ["DFF_Prop_fNoLineDrawDash",              BoolPropertyHandler,              "Draw a dashed line if no line"],
      
 512:  ["DFF_Prop_shadowType",                   LongPropertyHandler,              "Type of effect"],
 513:  ["DFF_Prop_shadowColor",                  ColorPropertyHandler,             "Foreground color"],
 514:  ["DFF_Prop_shadowHighlight",              ColorPropertyHandler,             "Embossed color"],
 515:  ["DFF_Prop_shadowCrMod",                  ColorPropertyHandler,             "Modification for BW views"],
 516:  ["DFF_Prop_shadowOpacity",                FixedPointHandler,        "Fixed 16.16"],
 517:  ["DFF_Prop_shadowOffsetX",                LongPropertyHandler,              "Offset shadow"],
 518:  ["DFF_Prop_shadowOffsetY",                LongPropertyHandler,              "Offset shadow"],
 519:  ["DFF_Prop_shadowSecondOffsetX",          LongPropertyHandler,              "Double offset shadow"],
 520:  ["DFF_Prop_shadowSecondOffsetY",          LongPropertyHandler,              "Double offset shadow"],
 521:  ["DFF_Prop_shadowScaleXToX",              FixedPointHandler,        "16.16"],
 522:  ["DFF_Prop_shadowScaleYToX",              FixedPointHandler,        "16.16"],
 523:  ["DFF_Prop_shadowScaleXToY",              FixedPointHandler,        "16.16"],
 524:  ["DFF_Prop_shadowScaleYToY",              FixedPointHandler,        "16.16"],
 525:  ["DFF_Prop_shadowPerspectiveX",           FixedPointHandler,        "16.16 / weight"],
 526:  ["DFF_Prop_shadowPerspectiveY",           FixedPointHandler,        "16.16 / weight"],
 527:  ["DFF_Prop_shadowWeight",                 LongPropertyHandler,              "scaling factor"],
 528:  ["DFF_Prop_shadowOriginX",                LongPropertyHandler, ""],
 529:  ["DFF_Prop_shadowOriginY",                LongPropertyHandler, ""],
 574:  ["DFF_Prop_fShadow",                      BoolPropertyHandler,              "Any shadow?"],
 575:  ["DFF_Prop_fshadowObscured",              BoolPropertyHandler,              "Excel5-style shadow"],
      
 576:  ["DFF_Prop_perspectiveType",              LongPropertyHandler,              "Where transform applies"],
 577:  ["DFF_Prop_perspectiveOffsetX",           LongPropertyHandler,              "The LONG values define a transformation matrix, effectively, each value is scaled by the perspectiveWeight parameter."],
 578:  ["DFF_Prop_perspectiveOffsetY",           LongPropertyHandler, ""],
 579:  ["DFF_Prop_perspectiveScaleXToX",         LongPropertyHandler, ""],
 580:  ["DFF_Prop_perspectiveScaleYToX",         LongPropertyHandler, ""],
 581:  ["DFF_Prop_perspectiveScaleXToY",         LongPropertyHandler, ""],
 582:  ["DFF_Prop_perspectiveScaleYToY",         LongPropertyHandler, ""],
 583:  ["DFF_Prop_perspectivePerspectiveX",      LongPropertyHandler, ""],
 584:  ["DFF_Prop_perspectivePerspectiveY",      LongPropertyHandler, ""],
 585:  ["DFF_Prop_perspectiveWeight",            LongPropertyHandler,              "Scaling factor"],
 586:  ["DFF_Prop_perspectiveOriginX",           LongPropertyHandler, ""],
 587:  ["DFF_Prop_perspectiveOriginY",           LongPropertyHandler, ""],
 639:  ["DFF_Prop_fPerspective",                 BoolPropertyHandler,              "On/off"],
      
 640:  ["DFF_Prop_c3DSpecularAmt",               FixedPointHandler,        "Fixed-point 16.16"],
 641:  ["DFF_Prop_c3DDiffuseAmt",                FixedPointHandler,        "Fixed-point 16.16"],
 642:  ["DFF_Prop_c3DShininess",                 LongPropertyHandler,              "Default gives OK results"],
 643:  ["DFF_Prop_c3DEdgeThickness",             LongPropertyHandler,              "Specular edge thickness"],
 644:  ["DFF_Prop_c3DExtrudeForward",            LongPropertyHandler,              "Distance of extrusion in EMUs"],
 645:  ["DFF_Prop_c3DExtrudeBackward",           LongPropertyHandler, ""],
 646:  ["DFF_Prop_c3DExtrudePlane",              LongPropertyHandler,              "Extrusion direction"],
 647:  ["DFF_Prop_c3DExtrusionColor",            ColorPropertyHandler,             "Basic color of extruded part of shape; the lighting model used will determine the exact shades used when rendering."],
 648:  ["DFF_Prop_c3DCrMod",                     ColorPropertyHandler,             "Modification for BW views"],
 700:  ["DFF_Prop_f3D",                          BoolPropertyHandler,              "Does this shape have a 3D effect?"],
 701:  ["DFF_Prop_fc3DMetallic",                 BoolPropertyHandler,              "Use metallic specularity?"],
 702:  ["DFF_Prop_fc3DUseExtrusionColor",        BoolPropertyHandler, ""],
 703:  ["DFF_Prop_fc3DLightFace",                BoolPropertyHandler, ""],
      
 704:  ["DFF_Prop_c3DYRotationAngle",            FixedPointHandler,        "degrees (16.16) about y axis"],
 705:  ["DFF_Prop_c3DXRotationAngle",            FixedPointHandler,        "degrees (16.16) about x axis"],
 706:  ["DFF_Prop_c3DRotationAxisX",             LongPropertyHandler,              "These specify the rotation axis; only their relative magnitudes matter."],
 707:  ["DFF_Prop_c3DRotationAxisY",             LongPropertyHandler, ""],
 708:  ["DFF_Prop_c3DRotationAxisZ",             LongPropertyHandler, ""],
 709:  ["DFF_Prop_c3DRotationAngle",             FixedPointHandler,        "degrees (16.16) about axis"],
 710:  ["DFF_Prop_c3DRotationCenterX",           FixedPointHandler,        "rotation center x (16.16 or g-units)"],
 711:  ["DFF_Prop_c3DRotationCenterY",           FixedPointHandler,        "rotation center y (16.16 or g-units)"],
 712:  ["DFF_Prop_c3DRotationCenterZ",           FixedPointHandler,        "rotation center z (absolute (emus))"],
 713:  ["DFF_Prop_c3DRenderMode",                LongPropertyHandler,              "Full,wireframe, or bcube"],
 714:  ["DFF_Prop_c3DTolerance",                 FixedPointHandler,        "pixels (16.16)"],
 715:  ["DFF_Prop_c3DXViewpoint",                LongPropertyHandler,              "X view point (emus)"],
 716:  ["DFF_Prop_c3DYViewpoint",                LongPropertyHandler,              "Y view point (emus)"],
 717:  ["DFF_Prop_c3DZViewpoint",                LongPropertyHandler,              "Z view distance (emus)"],
 718:  ["DFF_Prop_c3DOriginX",                   LongPropertyHandler, ""],
 719:  ["DFF_Prop_c3DOriginY",                   LongPropertyHandler, ""],
 720:  ["DFF_Prop_c3DSkewAngle",                 FixedPointHandler,        "degree (16.16) skew angle"],
 721:  ["DFF_Prop_c3DSkewAmount",                LongPropertyHandler,              "Percentage skew amount"],
 722:  ["DFF_Prop_c3DAmbientIntensity",          FixedPointHandler,        "Fixed point intensity"],
 723:  ["DFF_Prop_c3DKeyX",                      LongPropertyHandler,              "Key light source direction; only their relative magnitudes matter"],
 724:  ["DFF_Prop_c3DKeyY",                      LongPropertyHandler,              "Key light source direction; only their relative magnitudes matter"],
 725:  ["DFF_Prop_c3DKeyZ",                      LongPropertyHandler,              "Key light source direction; only their relative magnitudes matter"],
 726:  ["DFF_Prop_c3DKeyIntensity",              FixedPointHandler,        "Fixed point intensity"],
 727:  ["DFF_Prop_c3DFillX",                     LongPropertyHandler,              "Fill light source direction; only their relative magnitudes matter"],
 728:  ["DFF_Prop_c3DFillY",                     LongPropertyHandler,              "Fill light source direction; only their relative magnitudes matter"],
 729:  ["DFF_Prop_c3DFillZ",                     LongPropertyHandler,              "Fill light source direction; only their relative magnitudes matter"],
 730:  ["DFF_Prop_c3DFillIntensity",             FixedPointHandler,        "Fixed point intensity"],
 763:  ["DFF_Prop_fc3DConstrainRotation",        BoolPropertyHandler, ""],
 764:  ["DFF_Prop_fc3DRotationCenterAuto",       BoolPropertyHandler, ""],
 765:  ["DFF_Prop_fc3DParallel",                 BoolPropertyHandler,              "Parallel projection?"],
 766:  ["DFF_Prop_fc3DKeyHarsh",                 BoolPropertyHandler,              "Is key lighting harsh?"],
 767:  ["DFF_Prop_fc3DFillHarsh",                BoolPropertyHandler,              "Is fill lighting harsh?"],
      
 769:  ["DFF_Prop_hspMaster",                    LongPropertyHandler,              "master shape"],
 771:  ["DFF_Prop_cxstyle",                      LongPropertyHandler,              "Type of connector"],
 772:  ["DFF_Prop_bWMode",                       LongPropertyHandler,              "Settings for modifications to be made when in different forms of black-and-white mode."],
 773:  ["DFF_Prop_bWModePureBW",                 LongPropertyHandler, ""],
 774:  ["DFF_Prop_bWModeBW",                     LongPropertyHandler, ""],
 826:  ["DFF_Prop_fOleIcon",                     BoolPropertyHandler,              "For OLE objects, whether the object is in icon form"],
 827:  ["DFF_Prop_fPreferRelativeResize",        BoolPropertyHandler,              "For UI only. Prefer relative resizing."],
 828:  ["DFF_Prop_fLockShapeType",               BoolPropertyHandler,              "Lock the shape type (don't allow Change Shape)"],
 830:  ["DFF_Prop_fDeleteAttachedObject",        BoolPropertyHandler, ""],
 831:  ["DFF_Prop_fBackground",                  BoolPropertyHandler,              "If TRUE, this is the background shape."],
      
 832:  ["DFF_Prop_spcot",                        LongPropertyHandler,              "Callout type"],
 833:  ["DFF_Prop_dxyCalloutGap",                LongPropertyHandler,              "Distance from box to first point.(EMUs)"],
 834:  ["DFF_Prop_spcoa",                        LongPropertyHandler,              "Callout angle (any, 30,45,60,90,0)"],
 835:  ["DFF_Prop_spcod",                        LongPropertyHandler,              "Callout drop type"],
 836:  ["DFF_Prop_dxyCalloutDropSpecified",      LongPropertyHandler,              "if mso_spcodSpecified, the actual drop distance"],
 837:  ["DFF_Prop_dxyCalloutLengthSpecified",    LongPropertyHandler,              "if fCalloutLengthSpecified, the actual distance"],
 889:  ["DFF_Prop_fCallout",                     BoolPropertyHandler,              "Is the shape a callout?"],
 890:  ["DFF_Prop_fCalloutAccentBar",            BoolPropertyHandler,              "does callout have accent bar"],
 891:  ["DFF_Prop_fCalloutTextBorder",           BoolPropertyHandler,              "does callout have a text border"],
 892:  ["DFF_Prop_fCalloutMinusX",               BoolPropertyHandler, ""],
 893:  ["DFF_Prop_fCalloutMinusY",               BoolPropertyHandler, ""],
 894:  ["DFF_Prop_fCalloutDropAuto",             BoolPropertyHandler,              "If true, then we occasionally invert the drop distance"],
 895:  ["DFF_Prop_fCalloutLengthSpecified",      BoolPropertyHandler,              "if true, we look at dxyCalloutLengthSpecified"],
      
 896:  ["DFF_Prop_wzName",                       UniCharPropertyHandler,            "Shape Name (present only if explicitly set)"],
 897:  ["DFF_Prop_wzDescription",                UniCharPropertyHandler,            "alternate text"],
 898:  ["DFF_Prop_pihlShape",                    LongPropertyHandler,              "The hyperlink in the shape."],
 899:  ["DFF_Prop_pWrapPolygonVertices",         MsoArrayPropertyHandler,         "The polygon that text will be wrapped around (Word)"],
 900:  ["DFF_Prop_dxWrapDistLeft",               LongPropertyHandler,              "Left wrapping distance from text (Word)"],
 901:  ["DFF_Prop_dyWrapDistTop",                LongPropertyHandler,              "Top wrapping distance from text (Word)"],
 902:  ["DFF_Prop_dxWrapDistRight",              LongPropertyHandler,              "Right wrapping distance from text (Word)"],
 903:  ["DFF_Prop_dyWrapDistBottom",             LongPropertyHandler,              "Bottom wrapping distance from text (Word)"],
 904:  ["DFF_Prop_lidRegroup",                   LongPropertyHandler,              "Regroup ID"],
 927:  ["DFF_Prop_tableProperties",             LongPropertyHandler, ""],
 928:  ["DFF_Prop_tableRowProperties",          LongPropertyHandler, ""],
 937:  ["DFF_Prop_xmlstuff",                     LongPropertyHandler, "Embedded ooxml"],
 953:  ["DFF_Prop_fEditedWrap",                  BoolPropertyHandler,              "Has the wrap polygon been edited?"],
 954:  ["DFF_Prop_fBehindDocument",              BoolPropertyHandler,              "Word-only (shape is behind text)"],
 955:  ["DFF_Prop_fOnDblClickNotify",            BoolPropertyHandler,              "Notify client on a double click"],
 956:  ["DFF_Prop_fIsButton",                    BoolPropertyHandler,              "A button shape (i.e., clicking performs an action). Set for shapes with attached hyperlinks or macros."],
 957:  ["DFF_Prop_fOneD",                        BoolPropertyHandler,              "1D adjustment"],
 958:  ["DFF_Prop_fHidden",                      BoolPropertyHandler,              "Do not display"],
 959:  ["DFF_Prop_fPrint",                       BoolPropertyHandler,              "Print this shape"]

}


