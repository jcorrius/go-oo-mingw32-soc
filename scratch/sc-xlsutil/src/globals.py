
import sys

def output (msg):
    sys.stdout.write(msg)


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


def char2byte (chars):
    bytes = []
    for c in chars:
        bytes.append(ord(c))
    return bytes


def getRawBytes (bytes, spaced=True):
    text = ''
    for b in bytes:
        if type(b) == type(''):
            b = ord(b)
        if len(text) == 0:
            text = "%2.2X"%b
        elif spaced:
            text = "%2.2X "%b + text
        else:
            text = "%2.2X"%b + text
    return text


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

