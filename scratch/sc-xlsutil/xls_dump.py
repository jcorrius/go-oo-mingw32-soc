#!/usr/bin/env python

import sys, os.path, getopt
sys.path.append(sys.path[0]+"/src")
import ole, stream, globals

from globals import error

def usage (exname):
    exname = os.path.basename(exname)
    msg = """Usage: %s [options] [xls file]

Options:
  --help        displays this help message.
"""%exname
    print msg


class XLDumper(object):

    def __init__ (self, filepath, params):
        self.filepath = filepath
        self.params = params

    def __printDirHeader (self, dirname, byteLen):
        dirname = globals.decodeName(dirname)
        print("")
        print("="*68)
        print("%s (size: %d bytes)"%(dirname, byteLen))
        print("-"*68)

    def dump (self):
        file = open(self.filepath, 'rb')
        strm = stream.XLStream(file.read(), self.params)
        file.close()
        strm.printStreamInfo()
        strm.printHeader()
        strm.printMSAT()
        strm.printSAT()
        strm.printSSAT()
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


def main (args):
    exname, args = args[0], args[1:]
    if len(args) < 1:
        print("takes at least one argument")
        usage(exname)
        return

    params = globals.Params()
    try:
        opts, args = getopt.getopt(args, "h", ["help", "debug"])
        for opt, arg in opts:
            if opt in ['-h', '--help']:
                usage(exname)
                return
            elif opt in ['--debug']:
                params.Debug = True
            else:
                error("unknown option %s\n"%opt)
                usage()

    except getopt.GetoptError:
        error("error parsing input options\n")
        usage(exname)
        return

    dumper = XLDumper(args[0], params)
    dumper.dump()

if __name__ == '__main__':
    main(sys.argv)
