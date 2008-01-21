
import sys
import stream

def output (msg):
    sys.stdout.write(msg)

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
    BigEndian = 1
    Unknown = 2

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

    def __init__ (self, chars):
        self.chars = chars
        self.MSAT = None

        self.docId = None
        self.uId = None
        self.revision = 0
        self.version = 0
        self.byteOrder = ByteOrder.Unknown
        self.minStrmSize = 0
        self.numSecMSAT = 0
        self.numSecSSAT = 0
        self.numSecSAT = 0
        self.secIDFirstMSAT = -2
        self.secSize = 512
        self.secSizeShort = 64

    def dumpBytes (self, initPos=0, quitAtBOF=True):
        # dump OLE header until it reaches BOF.
        i, bprev = initPos, 0
        labelPrinted = False
        while True:
            b = ord(self.chars[i])
            if quitAtBOF and bprev == 0x09 and b == 0x08:
                # BOF reached
                return i-1

            if i%2 != 0:
                if not labelPrinted:
                    print("\n\nThe rest of the bytes:")
                    labelPrinted = True
                output("%2.2X %2.2X "%(bprev, b))

            if (i+1)%16 == 0:
                output("\n")

            bprev = b
            i += 1

        return i

    def dumpSectorBytes (self, initPos, secSize):
        print('foo')
        for i in xrange(0, secSize):
            b = ord(self.chars[i+initPos])
            output("%2.2X "%b)
            if (i+1)%16 == 0:
                output("\n")

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
        print("revision: %d  version: %d"%(self.revision, self.version))

        # byte order
        output("byte order: ")
        if self.byteOrder == ByteOrder.LittleEndian:
            print("little endian")
        elif self.byteOrder == ByteOrder.BigEndian:
            print("big endian")
        else:
            print("unknown")

        # sector size (usually 512 bytes)
        print("Sector size: %d"%self.secSize)

        # short sector size (usually 64 bytes)
        print("Short sector size: %d"%self.secSizeShort)

        # total number of sectors in SAT (equals the number of sector IDs 
        # stored in the MSAT).
        print("Total number of sectors used in SAT: %d"%self.numSecSAT)

        # ???
        print("Section ID of the first sector of the directory stream: %d"%self.secIDFirstDirStrm)

        print("Minimum stream size: %d"%self.minStrmSize)

        print("SecID of first SSAT sector: %d"%(512+self.secIDFirstSSAT*self.secSizeShort))

        print("Total number of sectors used in SSAT: %d"%self.numSecSSAT)

        if self.secIDFirstMSAT == -2:
            # There is no more sector ID stored outside the header.
            print("SecID of first MSAT sector: [end of chain]")
        else:
            # There is more sector IDs than 109 IDs stored in the header.
            print("SecID of first MSAT sector: %d"%(512+self.secIDFirstMSAT*self.secSize))

        print("Total number of sectors used to store additional MSAT: %d"%self.numSecMSAT)

    def parse (self):

        # document ID and unique ID
        self.docId = self.chars[0:8]
        self.uId = self.chars[8:24]

        # revision and version
        self.revision = getSignedInt(self.chars[24:26])
        self.version = getSignedInt(self.chars[26:28])

        # byte order
        self.byteOrder = Header.byteOrder(self.chars[28:30])

        # sector size (usually 512 bytes)
        self.secSize = 2**getSignedInt(self.chars[30:32])

        # short sector size (usually 64 bytes)
        self.secSizeShort = 2**getSignedInt(self.chars[32:34])

        # total number of sectors in SAT (equals the number of sector IDs 
        # stored in the MSAT).
        self.numSecSAT = getSignedInt(self.chars[44:48])

        self.secIDFirstDirStrm = getSignedInt(self.chars[48:52])
        self.minStrmSize = getSignedInt(self.chars[56:60])
        self.secIDFirstSSAT = getSignedInt(self.chars[60:64])
        self.numSecSSAT = getSignedInt(self.chars[64:68])
        self.secIDFirstMSAT = getSignedInt(self.chars[68:72])
        self.numSecMSAT = getSignedInt(self.chars[72:76])

        self.MSAT = MSAT(self.secSize)

        # First part of MSAT consisting of an array of up to 109 sector IDs.
        # Each sector ID is 4 bytes in length.
        for i in xrange(0, 109):
            pos = 76 + i*4
            id = getSignedInt(self.chars[pos:pos+4])
            if id == -1:
                break

            self.MSAT.appendSectorID(id)

        return 512
#       return self.dumpBytes(512)

    def dumpSAT (self):
        for pos in self.secPosList:
            self.dumpSectorBytes(pos, 512)
        return

    def getMSAT (self):
        return self.MSAT


class MSAT(object):
    """Master Sector Allocation Table

This class represents the master sector allocation table (MSAT) that
stores sector IDs that point to the sectors that are used by the sector
allocation table (SAT).  The actual SAT are to be constructed by 
combining all the sectors pointed by the sector IDs in order of 
occurrence.
"""
    def __init__ (self, sectorSize):
        self.sectorSize = sectorSize
        self.secIDs = []

    def appendSectorID (self, id):
        self.secIDs.append(id)

    def output (self):
        print('')
        print("="*68)
        print("Master Sector Allocation Table")
        print("-"*68)

        for id in self.secIDs:
            print("sector ID: %5d   (pos: %7d)"%(id, 512+id*self.sectorSize))




