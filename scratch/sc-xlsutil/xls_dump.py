#!/usr/bin/env python

import sys, os.path
sys.path.append(sys.path[0]+"/src")
import ole, stream, globals

class XLDumper(object):

    def __init__ (self, filepath):
        self.filepath = filepath

    def __printDirHeader (self, dirname, byteLen):
        if ord(dirname[0]) <= 5:
            dirname = "<%2.2Xh>%s"%(ord(dirname[0]), dirname[1:])
        print("")
        print("="*68)
        print("%s (size: %d bytes)"%(dirname, byteLen))
        print("-"*68)

    def dump (self):
        file = open(self.filepath, 'rb')
        strm = stream.XLStream(file.read())
        file.close()
        strm.printStreamInfo()
        strm.printHeader()
        strm.printMSAT()
        strm.printSAT()
#       strm.printSSAT()
        strm.printDirectory()
        dirnames = strm.getDirectoryNames()
        for dirname in dirnames:
            if len(dirname) == 0 or dirname == 'Root Entry':
                continue

            dirstrm = strm.getDirectoryStreamByName(dirname)
            self.__printDirHeader(dirname, len(dirstrm.bytes))
            if dirname == "Workbook":
                success = True
                while success: 
                    success = self.__readSheetSubStream(dirstrm)
            elif dirname == "Revision Log":
                dirstrm.type = stream.DirType.RevisionLog
                try:
                    header = 0x0000
                    while header != 0x000A:
                        header = dirstrm.readRecord()
                except stream.EndOfStream:
                    continue
            else:
                globals.dumpBytes(dirstrm.bytes, 512)


    def __readSheetSubStream (self, strm):
        # read bytes from BOF to EOF.
        strm.seekBOF()
        try:
            header = 0x0000
            while header != 0x000A:
                header = strm.readRecord()
            return True

        except stream.EndOfStream:
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
