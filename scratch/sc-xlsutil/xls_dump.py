#!/usr/bin/env python

import sys, os.path
sys.path.append(sys.path[0]+"/src")
import ole, stream

class XLDumper(object):

    def __init__ (self, filepath):
        self.filepath = filepath

    def dump (self):
        file = open(self.filepath, 'r')
        strm = stream.XLStream(file.read())
        file.close()
        strm.printStreamInfo()
        strm.printHeader()
#       strm.printMSAT()
#       strm.printSAT()
#       strm.printSSAT()
        strm.printDirectory()
        success = True
        while success: 
            success = self.__read(strm)

    def __read (self, strm):
        # read bytes from BOF to EOF.
        strm.seekBOF()
        try:
            header = 0x0000
            while header != 0x000A:
                header = strm.readRecord()
            return True

        except IndexError:
            return False

def usage ():
    pass

def main (args):
    if len(args) < 1:
        print("takes at least one argument")
        usage()
        return

    dumper = XLDumper(args[0])
    dumper.dump()

if __name__ == '__main__':
    main(sys.argv[1:])
