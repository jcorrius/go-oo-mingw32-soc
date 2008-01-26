
import sys, struct

class ByteConvertError(Exception): pass

def output (msg):
    sys.stdout.write(msg)

def error (msg):
    sys.stderr.write(msg)

def dumpBytes (chars, subDivide=None):
    line = 0
    subDivideLine = None
    if subDivide != None:
        subDivideLine = subDivide/16
    for i in xrange(0, len(chars)):
        byte = ord(chars[i])
        output("%2.2X "%byte)
        if (i+1)%16 == 0:
            output("\n")
            if subDivideLine != None and (line+1)%subDivideLine == 0:
                output("\n")
            line += 1

    output("\n")


def getSectorPos (secID, secSize):
    return 512 + secID*secSize


def getRawBytes (bytes, spaced=True, reverse=True):
    text = ''
    for b in bytes:
        if type(b) == type(''):
            b = ord(b)
        if len(text) == 0:
            text = "%2.2X"%b
        elif spaced:
            if reverse:
                text = "%2.2X "%b + text
            else:
                text += " %2.2X"%b
        else:
            if reverse:
                text = "%2.2X"%b + text
            else:
                text += "%2.2X"%b
    return text


def toTextBytes (bytes):
    n = len(bytes)
    text = ''
    for i in xrange(0, n):
        b = bytes[i]
        if type(b) == type(0x00):
            b = struct.pack('B', b)
        text += b
    return text


def getSignedInt (bytes):
    # little endian
    n = len(bytes)
    if n == 0:
        return 0

    text = toTextBytes(bytes)
    if n == 1:
        # byte - 1 byte
        return struct.unpack('b', text)[0]
    elif n == 2:
        # short - 2 bytes
        return struct.unpack('h', text)[0]
    elif n == 4:
        # int, long - 4 bytes
        return struct.unpack('l', text)[0]

    raise ByteConvertError


def getDouble (bytes):
    n = len(bytes)
    if n == 0:
        return 0.0

    text = toTextBytes(bytes)
    return struct.unpack('d', text)[0]


def getUTF8FromUTF16 (bytes):
    # little endian utf-16 strings
    byteCount = len(bytes)
    loopCount = int(byteCount/2)
    text = ''
    for i in xrange(0, loopCount):
        code = ''
        if bytes[i*2+1] != '\x00':
            code += bytes[i*2+1]
        if bytes[i*2] != '\x00':
            code += bytes[i*2]
        text += unicode(code, 'utf-8')
    return text

