#!/usr/bin/env python

import sys, os, os.path, zipfile, getopt

def error (msg):
    sys.stderr.write(msg + "\n")
    sys.exit(1)

def stripPath (file, paths):
    for _path in paths:
        if file.find(_path) == 0:
            return file[len(_path):]
    return file

def isHidden (filepath):

    # TODO: how do I get an OS specific separator?
    for name in filepath.split('/'): 
        if name in ['..', '.']:
            continue

        if len(name) > 1 and name[0] == '.' and name[1] != '.':
            return True

    return False

def expandDirTree (_files):
    _files2 = []
    for _file in _files:
        if os.path.isfile(_file):
            # this is a file.  add it without further processing.
            _files2.append(_file)
            continue

        if not os.path.isdir(_file):
            # not a directory.  skip it.
            continue

        for root, dirs, files in os.walk(_file):
            for _dirfile in files:
                _dirfile = root + '/' + _dirfile
                if not isHidden(_dirfile):
                    _files2.append(_dirfile)

    return _files2

def pack (arcfile, files, paths):
#   print("archiving to "+ arcfile)
    zipobj = zipfile.ZipFile(arcfile, 'w')
    files = expandDirTree(files)
    for file in files:
        arcname = stripPath(file, paths)
        zipobj.write(file, arcname)
#       print("  adding file " + file + " -> " + arcname)
    zipobj.close()

def main (args):

    try:
        opts, args = getopt.getopt(args, "p:", ["path="])
    except getopt.GetoptError:
        error("error parsing input options")

    paths = []
    for opt, arg in opts:
        if opt in ['-p', '--path']:
            # append base path
            paths.append(arg)
        else:
            error("unknown option " + opt)

    if len(args) < 2:
        error("requires at least 2 arguments")

    arcfile, files = args[0], args[1:]
    pack(args[0], args[1:], paths)

if __name__ == '__main__':
    main(sys.argv[1:])
