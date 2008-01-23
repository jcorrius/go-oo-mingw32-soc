
import sys
import stream, globals

# ----------------------------------------------------------------------------
# Reference: The Microsoft Compound Document File Format by Daniel Rentz
# http://sc.openoffice.org/compdocfileformat.pdf
# ----------------------------------------------------------------------------

from globals import output

def printSep (c='-', w=68, prefix=''):
    print(prefix + c*w)

def char2byte (chars):
    bytes = []
    for c in chars:
        bytes.append(ord(c))
    return bytes

def getSignedInt (bytes):
    # little endian
    n = len(bytes)
    if n == 0:
        return 0

    if type(bytes[0]) == type('c'):
        bytes = char2byte(bytes)

    isNegative = (bytes[-1] & 0x80) == 0x80

    num, ff = 0, 0
    for i in xrange(0, n):
        num += bytes[i]*(256**i)
        ff += 0xFF*(256**i)
        i += 1

    if isNegative:
        # perform two's compliment.
        num = -((num^ff) + 1)

    return num

class ByteOrder:
    LittleEndian = 0
    BigEndian    = 1
    Unknown      = 2

class SectorType:
    MSAT      = 0
    SAT       = 1
    SSAT      = 2
    Directory = 3

class StreamLocation:
    SAT  = 0
    SSAT = 1

class Header(object):

    @staticmethod
    def byteOrder (chars):
        b1, b2 = ord(chars[0]), ord(chars[1])
        if b1 == 0xFE and b2 == 0xFF:
            return ByteOrder.LittleEndian
        elif b1 == 0xFF and b2 == 0xFE:
            return ByteOrder.BigEndian
        else:
            return ByteOrder.Unknown


    def __init__ (self, bytes):
        self.bytes = bytes
        self.MSAT = None

        self.docId = None
        self.uId = None
        self.revision = 0
        self.version = 0
        self.byteOrder = ByteOrder.Unknown
        self.minStreamSize = 0

        self.numSecMSAT = 0
        self.numSecSSAT = 0
        self.numSecSAT = 0

        self.__secIDFirstMSAT = -2
        self.__secIDFirstDirStrm = -2
        self.__secIDFirstSSAT = -2

        self.secSize = 512
        self.secSizeShort = 64


    def getSectorSize (self):
        return 2**self.secSize


    def getShortSectorSize (self):
        return 2**self.secSizeShort


    def getFirstSectorID (self, sectorType):
        if sectorType == SectorType.MSAT:
            return self.__secIDFirstMSAT
        elif sectorType == SectorType.SSAT:
            return self.__secIDFirstSSAT
        elif sectorType == SectorType.Directory:
            return self.__secIDFirstDirStrm
        return -2


    def output (self):

        def printRawBytes (bytes):
            for b in bytes:
                output("%2.2X "%ord(b))
            output("\n")

        printSep('=', 68)
        print("Compound Document Header")
        printSep('-', 68)

        # document ID and unique ID
        output("Document ID: ")
        printRawBytes(self.docId)
        output("Unique ID: ")
        printRawBytes(self.uId)

        # revision and version
        print("Revision: %d  Version: %d"%(self.revision, self.version))

        # byte order
        output("Byte order: ")
        if self.byteOrder == ByteOrder.LittleEndian:
            print("little endian")
        elif self.byteOrder == ByteOrder.BigEndian:
            print("big endian")
        else:
            print("unknown")

        # sector size (usually 512 bytes)
        print("Sector size: %d (%d)"%(2**self.secSize, self.secSize))

        # short sector size (usually 64 bytes)
        print("Short sector size: %d (%d)"%(2**self.secSizeShort, self.secSizeShort))

        # total number of sectors in SAT (equals the number of sector IDs 
        # stored in the MSAT).
        print("Total number of sectors used in SAT: %d"%self.numSecSAT)

        print("Sector ID of the first sector of the directory stream: %d"%
              self.__secIDFirstDirStrm)

        print("Minimum stream size: %d"%self.minStreamSize)

        if self.__secIDFirstSSAT == -2:
            print("Sector ID of the first SSAT sector: [none]")
        else:
            print("Sector ID of the first SSAT sector: %d"%self.__secIDFirstSSAT)

        print("Total number of sectors used in SSAT: %d"%self.numSecSSAT)

        if self.__secIDFirstMSAT == -2:
            # There is no more sector ID stored outside the header.
            print("Sector ID of the first MSAT sector: [end of chain]")
        else:
            # There is more sector IDs than 109 IDs stored in the header.
            print("Sector ID of the first MSAT sector: %d"%(512+self.__secIDFirstMSAT*(2**self.secSize)))

        print("Total number of sectors used to store additional MSAT: %d"%self.numSecMSAT)


    def parse (self):

        # document ID and unique ID
        self.docId = self.bytes[0:8]
        self.uId = self.bytes[8:24]

        # revision and version
        self.revision = getSignedInt(self.bytes[24:26])
        self.version = getSignedInt(self.bytes[26:28])

        # byte order
        self.byteOrder = Header.byteOrder(self.bytes[28:30])

        # sector size (usually 512 bytes)
        self.secSize = getSignedInt(self.bytes[30:32])

        # short sector size (usually 64 bytes)
        self.secSizeShort = getSignedInt(self.bytes[32:34])

        # total number of sectors in SAT (equals the number of sector IDs 
        # stored in the MSAT).
        self.numSecSAT = getSignedInt(self.bytes[44:48])

        self.__secIDFirstDirStrm = getSignedInt(self.bytes[48:52])
        self.minStreamSize = getSignedInt(self.bytes[56:60])
        self.__secIDFirstSSAT = getSignedInt(self.bytes[60:64])
        self.numSecSSAT = getSignedInt(self.bytes[64:68])
        self.__secIDFirstMSAT = getSignedInt(self.bytes[68:72])
        self.numSecMSAT = getSignedInt(self.bytes[72:76])

        # master sector allocation table
        self.MSAT = MSAT(2**self.secSize, self.bytes)

        # First part of MSAT consisting of an array of up to 109 sector IDs.
        # Each sector ID is 4 bytes in length.
        for i in xrange(0, 109):
            pos = 76 + i*4
            id = getSignedInt(self.bytes[pos:pos+4])
            if id == -1:
                break

            self.MSAT.appendSectorID(id)

        return 512


    def getMSAT (self):
        return self.MSAT


    def getSAT (self):
        return self.MSAT.getSAT()


    def getSSAT (self):
        ssatID = self.getFirstSectorID(SectorType.SSAT)
        if ssatID < 0:
            return None
        chain = self.getSAT().getSectorIDChain(ssatID)
        if len(chain) == 0:
            return None
        obj = SSAT(2**self.secSize, self.bytes)
        for secID in chain:
            obj.addSector(secID)
        obj.buildArray()
        return obj


    def getDirectory (self):
        dirID = self.getFirstSectorID(SectorType.Directory)
        if dirID < 0:
            return None
        chain = self.getSAT().getSectorIDChain(dirID)
        if len(chain) == 0:
            return None
        obj = Directory(self)
        for secID in chain:
            obj.addSector(secID)
        return obj


    def dummy ():
        pass




class MSAT(object):
    """Master Sector Allocation Table (MSAT)

This class represents the master sector allocation table (MSAT) that stores 
sector IDs that point to all the sectors that are used by the sector 
allocation table (SAT).  The actual SAT are to be constructed by combining 
all the sectors pointed by the sector IDs in order of occurrence.
"""
    def __init__ (self, sectorSize, bytes):
        self.sectorSize = sectorSize
        self.secIDs = []
        self.bytes = bytes
        self.__SAT = None

    def appendSectorID (self, id):
        self.secIDs.append(id)

    def output (self):
        print('')
        print("="*68)
        print("Master Sector Allocation Table (MSAT)")
        print("-"*68)

        for id in self.secIDs:
            print("sector ID: %5d   (pos: %7d)"%(id, 512+id*self.sectorSize))

    def getSATSectorPosList (self):
        list = []
        for id in self.secIDs:
            pos = 512 + id*self.sectorSize
            list.append([id, pos])
        return list

    def getSAT (self):
        if self.__SAT != None:
            return self.__SAT

        obj = SAT(self.sectorSize, self.bytes)
        for id in self.secIDs:
            obj.addSector(id)
        obj.buildArray()
        self.__SAT = obj
        return self.__SAT


class SAT(object):
    """Sector Allocation Table (SAT)
"""
    def __init__ (self, sectorSize, bytes):
        self.sectorSize = sectorSize
        self.sectorIDs = []
        self.bytes = bytes
        self.array = []


    def getSectorSize (self):
        return self.sectorSize


    def addSector (self, id):
        self.sectorIDs.append(id)


    def buildArray (self):
        numItems = int(self.sectorSize/4)
        self.array = []
        for secID in self.sectorIDs:
            pos = 512 + secID*self.sectorSize
            for i in xrange(0, numItems):
                beginPos = pos + i*4
                id = getSignedInt(self.bytes[beginPos:beginPos+4])
                self.array.append(id)


    def output (self):
        print('')
        print("="*68)
        print("Sector Allocation Table (SAT)")
        print("-"*68)
        for i in xrange(0, len(self.array)):
            item = self.array[i]
            output("%3d : %3d\n"%(i, item))


    def getSectorIDChain (self, initID):
        if initID < 0:
            return []
        chain = [initID]
        nextID = self.array[initID]
        while nextID != -2:
            chain.append(nextID)
            nextID = self.array[nextID]
        return chain


class SSAT(SAT):
    """Short Sector Allocation Table (SSAT)

SSAT contains an array of sector ID chains of all short streams, as oppposed 
to SAT which contains an array of sector ID chains of all standard streams.
The sector IDs included in the SSAT point to the short sectors in the short
stream container stream.

The first sector ID of SSAT is in the header, and the IDs of the remaining 
sectors are contained in the SAT as a sector ID chain.
"""

    def __init__ (self, sectorSize, bytes):
        SAT.__init__(self, sectorSize, bytes)
        return

    def output (self):
        print('')
        print("="*68)
        print("Short Sector Allocation Table (SSAT)")
        print("-"*68)
        for i in xrange(0, len(self.array)):
            item = self.array[i]
            output("%3d : %3d\n"%(i, item))


class Directory(object):

    class Type:
        Empty = 0
        UserStorage = 1
        UserStream = 2
        LockBytes = 3
        Property = 4
        RootStorage = 5

    class NodeColor:
        Red = 0
        Black = 1
        Unknown = 99
        
    class Entry:
        def __init__ (self):
            self.Name = ''
            self.CharBufferSize = 0
            self.Type = Directory.Type.Empty
            self.NodeColor = Directory.NodeColor.Unknown
            self.DirIDLeft = -1
            self.DirIDRight = -1
            self.DirIDRoot = -1
            self.UniqueID = None
            self.UserFlags = None
            self.TimeCreated = None
            self.TimeModified = None
            self.StreamSectorID = -2
            self.StreamSize = 0


    def __init__ (self, header):
        self.sectorSize = header.getSectorSize()
        self.bytes = header.bytes
        self.minStreamSize = header.minStreamSize
        self.sectorIDs = []
        self.entries = []
        self.SAT = header.getSAT()
        self.SSAT = header.getSSAT()
        self.header = header
        self.posRootStorage = None


    def __getRawStream (self, entry):
        chain = []
        if entry.StreamLocation == StreamLocation.SAT:
            chain = self.header.getSAT().getSectorIDChain(entry.StreamSectorID)
        elif entry.StreamLocation == StreamLocation.SSAT:
            chain = self.header.getSSAT().getSectorIDChain(entry.StreamSectorID)

        offset = 512
        size = self.header.getSectorSize()
        bytes = []
        if entry.StreamLocation == StreamLocation.SSAT:
            # get root storage position
            offset = self.posRootStorage
            size = self.header.getShortSectorSize()

        for id in chain:
            pos = offset + id*size
            bytes.extend(self.header.bytes[pos:pos+size])

        return bytes


    def getRawStreamByName (self, name):
        bytes = []
        for entry in self.entries:
            if entry.Name == name:
                bytes = self.__getRawStream(entry)
                break
        return bytes


    def addSector (self, id):
        self.sectorIDs.append(id)


    def output (self, debug=False):
        print('')
        print("="*68)
        print("Directory")

        if debug:
            print("-"*68)
            print("sector(s) used:")
            for secID in self.sectorIDs:
                print("  sector %d"%secID)

            print("")
            for secID in self.sectorIDs:
                print("-"*68)
                print("  Raw Hex Dump (sector %d)"%secID)
                print("-"*68)
                pos = globals.getSectorPos(secID, self.sectorSize)
                globals.dumpBytes(self.bytes[pos:pos+self.sectorSize], 128)

        for entry in self.entries:
            self.__outputEntry(entry, debug)

    def __outputEntry (self, entry, debug):
        print("-"*68)
        if len(entry.Name) > 0:
            name = entry.Name
            if ord(name[0]) <= 5:
                name = "<%2.2Xh>%s"%(ord(name[0]), name[1:])
            print("name: %s   (name buffer size: %d bytes)"%(name, entry.CharBufferSize))
        else:
            print("name: [empty]   (name buffer size: %d bytes)"%entry.CharBufferSize)

        output("type: ")
        if entry.Type == Directory.Type.Empty:
            print("empty")
        elif entry.Type == Directory.Type.LockBytes:
            print("lock bytes")
        elif entry.Type == Directory.Type.Property:
            print("property")
        elif entry.Type == Directory.Type.RootStorage:
            print("root storage")
        elif entry.Type == Directory.Type.UserStorage:
            print("user storage")
        elif entry.Type == Directory.Type.UserStream:
            print("user stream")
        else:
            print("[unknown type]")

        output("node color: ")
        if entry.NodeColor == Directory.NodeColor.Red:
            print("red")
        elif entry.NodeColor == Directory.NodeColor.Black:
            print("black")
        elif entry.NodeColor == Directory.NodeColor.Unknown:
            print("[unknown color]")

        print("linked dir entries: left: %d; right: %d; root: %d"%
              (entry.DirIDLeft, entry.DirIDRight, entry.DirIDRoot))

        self.__outputRaw("unique ID",  entry.UniqueID)
        self.__outputRaw("user flags", entry.UserFlags)
        self.__outputRaw("time created", entry.TimeCreated)
        self.__outputRaw("time last modified", entry.TimeModified)

        output("stream info: ")
        if entry.StreamSectorID < 0:
            print("[empty stream]")
        else:
            strmLoc = "SAT"
            if entry.StreamLocation == StreamLocation.SSAT:
                strmLoc = "SSAT"
            print("(first sector ID: %d; size: %d; location: %s)"%
                  (entry.StreamSectorID, entry.StreamSize, strmLoc))
    
            satObj = None
            secSize = 0
            if entry.StreamLocation == StreamLocation.SAT:
                satObj = self.SAT
                secSize = self.header.getSectorSize()
            elif entry.StreamLocation == StreamLocation.SSAT:
                satObj = self.SSAT
                secSize = self.header.getShortSectorSize()
            if satObj != None:
                chain = satObj.getSectorIDChain(entry.StreamSectorID)
                print("sector count: %d"%len(chain))
                print("total sector size: %d"%(len(chain)*secSize))
                self.__outputSectorChain(chain)



    def __outputSectorChain (self, chain):
        line = "sector chain: "
        lineLen = len(line)
        for id in chain:
            frag = "%d, "%id
            fragLen = len(frag)
            if lineLen + fragLen > 68:
                print(line)
                line = frag
                lineLen = fragLen
            else:
                line += frag
                lineLen += fragLen
        if line[-2:] == ", ":
            line = line[:-2]
            lineLen -= 2
        if lineLen > 0:
            print(line)


    def __outputRaw (self, name, bytes):
        if bytes == None:
            return

        output("%s: "%name)
        for byte in bytes:
            output("%2.2X "%ord(byte))
        print("")


    def getDirectoryNames (self):
        names = []
        for entry in self.entries:
            names.append(entry.Name)
        return names


    def parseDirEntries (self):
        if len(self.entries):
            # directory entries already built
            return

        # combine all sectors first.
        bytes = []
        for secID in self.sectorIDs:
            pos = globals.getSectorPos(secID, self.sectorSize)
            bytes.extend(self.bytes[pos:pos+self.sectorSize])

        self.entries = []

        # each directory entry is exactly 128 bytes.
        numEntries = int(len(bytes)/128)
        if numEntries == 0:
            return
        for i in xrange(0, numEntries):
            pos = i*128
            self.entries.append(self.parseDirEntry(bytes[pos:pos+128]))


    def parseDirEntry (self, bytes):
        entry = Directory.Entry()
        name = globals.getUTF8FromUTF16(bytes[0:64])
        entry.Name = name
        entry.CharBufferSize = getSignedInt(bytes[64:66])
        entry.Type = getSignedInt(bytes[66:67])
        entry.NodeColor = getSignedInt(bytes[67:68])

        entry.DirIDLeft  = getSignedInt(bytes[68:72])
        entry.DirIDRight = getSignedInt(bytes[72:76])
        entry.DirIDRoot  = getSignedInt(bytes[76:80])

        entry.UniqueID     = bytes[80:96]
        entry.UserFlags    = bytes[96:100]
        entry.TimeCreated  = bytes[100:108]
        entry.TimeModified = bytes[108:116]

        entry.StreamSectorID = getSignedInt(bytes[116:120])
        entry.StreamSize     = getSignedInt(bytes[120:124])
        entry.StreamLocation = StreamLocation.SAT
        if entry.Type != Directory.Type.RootStorage and \
            entry.StreamSize < self.header.minStreamSize:
            entry.StreamLocation = StreamLocation.SSAT

        if entry.Type == Directory.Type.RootStorage and entry.StreamSectorID >= 0:
            self.posRootStorage = 512 + entry.StreamSectorID*self.header.getSectorSize()

        return entry

