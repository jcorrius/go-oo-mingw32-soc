#!/usr/bin/env python

import sys, getopt, os, os.path
from srclexer import SrcLexer
from srcparser import SrcParser
from boxer import Boxer
from globals import *

def usage (quitAfter=True):
    msg = """usage: src2xml.py [options] file1 file2 ....

Options:

    --output-dir        output directory where the converted xml files are 
                        stored.  The names of the output files will be 
                       
                          [basename].xml
                       
                        where the [basename] is the base name of the original 
                        source file.

    --include-dir       specifiy a header search path where header files are
                        located.  In case of multiple header search paths, use
                        this option multiple times.

    --include-dir-list  specify a file that contains header search paths.  
                        Each line in the specified file must correspond with
                        each individual header search path.

    --file-list         specify a file that contains the source file paths.
                        Each line in the specified file must correspond with
                        each individual source file path.
"""
    print msg
    if quitAfter:
        sys.exit(1)

class Params(object):
    """Container for command-line parameters.
"""
    def __init__ (self):
        self.debugLexer = False
        self.debugParser = False
        self.debugMacro = False
        self.outputDir = None
        self.dryRun = False
        self.onlyExpandMacros = False
        self.stopOnHeader = False
        self.stopOnFailure = False
        self.ignoreHeaders = False
        self.filelist = []
        self.includeDirs = {}

def convert (filepath, params):

    fullpath = os.path.abspath(filepath)
    if not os.path.isfile(fullpath):
        error("no such file", True)

    params.includeDirs[os.path.dirname(fullpath)] = True
    chars = open(fullpath, 'r').read()
    lexer = SrcLexer(chars, fullpath)
    lexer.expandHeaders = not params.ignoreHeaders
    lexer.includeDirs = params.includeDirs
    lexer.stopOnHeader = params.stopOnHeader
    lexer.debugMacro = params.debugMacro
#     lexer.debug = True
    if params.debugLexer:
        lexer.debug = True
        lexer.tokenize()
        output("-"*68 + "\n")
        output("** token dump\n")
        lexer.dumpTokens()
        output("** end of token dump\n")
        return

    # Tokenize it using lexer
    lexer.tokenize()

    parser = SrcParser(lexer.getTokens(), lexer.getDefines())
    parser.onlyExpandMacros = params.onlyExpandMacros
    if params.debugParser:
        parser.debug = True
        root = parser.parse()
        print root.dump()
        return

    # Parse the tokens.
    root = parser.parse()

    # Box it, and print out the XML tree.
    root = Boxer(root).layout()
    chars = root.dump()

    if params.dryRun:
        return

    if params.outputDir != None:
        outpath = os.path.basename(fullpath)
        root, ext = os.path.splitext(outpath)
        outpath = params.outputDir + '/' + root + '.xml'
        open(outpath, 'w').write(chars)
    else:
        print chars


def getAbsPath (inputPath, isDir=False, quitOnError=True):
    absPath = os.path.expanduser(inputPath)
    absPath = os.path.abspath(absPath)
    if isDir:
        if not os.path.isdir(absPath):
            error("%s is not a directory\n"%absPath, quitOnError)
    else:
        if not os.path.isfile(absPath):
            error("%s is not a file\n"%absPath, quitOnError)
    return absPath


def main (args):
    try:
        opts, args = getopt.getopt(args, "d:", 
                                   ["dry-run", "debug-macro", "debug-lexer", 
                                    "debug-parser", "output-dir=", 
                                    "include-dir=", "include-dir-list=", 
                                    "file-list=",
                                    "stop-on-failure",
                                    "stop-on-header",
                                    "ignore-headers",
                                    "only-expand-macros"])
    except getopt.GetoptError:
        error("error parsing input options\n")
        usage()

    params = Params()

    for opt, arg in opts:
        if opt == '--debug-lexer':
            params.debugLexer = True
        elif opt == '--debug-parser':
            params.debugParser = True
        elif opt == '--debug-macro':
            params.debugMacro = True
        elif opt == '--dry-run':
            params.dryRun = True
        elif opt == '--only-expand-macros':
            params.onlyExpandMacros = True
        elif opt == '--stop-on-failure':
            params.stopOnFailure = True
        elif opt == '--stop-on-header':
            params.stopOnHeader = True
        elif opt == '--ignore-headers':
            params.ignoreHeaders = True
        elif opt in ['-d', '--output-dir']:
            arg = getAbsPath(arg, True, True)
            params.outputDir = arg
        elif opt == '--include-dir':
            arg = getAbsPath(arg, True, True)
            params.includeDirs[arg] = True
        elif opt == '--include-dir-list':
            arg = getAbsPath(arg)
            for line in open(arg, 'r').readlines():
                line = os.path.expanduser(line)
                line = os.path.abspath(line).strip()
                if os.path.isdir(line):
                    params.includeDirs[line] = True
                else:
                    error("%s is not a directory\n"%line)
        elif opt == '--file-list':
            arg = getAbsPath(arg)
            for line in open(arg, 'r').readlines():
                line = os.path.expanduser(line).rstrip()
                line = os.path.abspath(line)
                params.filelist.append(line)
        else:
            error("unknown option %s\n"%opt)
            usage()

    if len(args) == 0 and len(params.filelist) == 0:
        error("no input files\n")
        usage()

    for arg in args:
        params.filelist.append(arg)

    for fpath in params.filelist:
        output("parsing " + fpath)
        if params.dryRun:
            try:
                convert(fpath, params)
                output("  SUCCESS\n")
            except:
                output("  FAILED\n")
                if params.stopOnFailure:
                    sys.exit(1)
        else:
            output("\n")
            convert(fpath, params)


if __name__ == '__main__':
    main(sys.argv[1:])
