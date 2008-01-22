
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

