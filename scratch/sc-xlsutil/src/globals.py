
import sys

def output (msg):
    sys.stdout.write(msg)

def dumpBytes (chars):
    for i in xrange(0, len(chars)):
        byte = ord(chars[i])
        output("%2.2X "%byte)
        if (i+1)%16 == 0:
            output("\n")
    output("\n")

