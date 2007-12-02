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

def addDirectoryToArchive (zipobj, name):
    dirobj = zipfile.ZipInfo(name)
    # without setting external attribute to the following value, UNO registration
    # fails on Linux.
    dirobj.external_attr = 1106051088
    zipobj.writestr(dirobj, '')

def expandDirTree (_files):
    _files2, dirs2 = [], []
    for _file in _files:
        if os.path.isfile(_file):
            # this is a file.  add it without further processing.
            _files2.append(_file)
            continue

        if not os.path.isdir(_file):
            # not a directory.  skip it.
            continue

        for root, dirs, files in os.walk(_file):
            if not isHidden(root):
                dirs2.append(root)
            for _dirfile in files:
                _dirfile = root + '/' + _dirfile
                if not isHidden(_dirfile):
                    _files2.append(_dirfile)

    return _files2, dirs2

def pack (arcfile, files, paths):
    zipobj = zipfile.ZipFile(arcfile, 'w', zipfile.ZIP_DEFLATED)
    files, dirs = expandDirTree(files)

    dirs.sort()
    for _dir in dirs:
        _dir = stripPath(_dir, paths)
        _dir = _dir[1:]+'/'
        print("  adding " + _dir)
        addDirectoryToArchive(zipobj, _dir)

    files.sort()
    for file in files:
        arcname = stripPath(file, paths)[1:]
        zipobj.write(file, arcname, zipfile.ZIP_DEFLATED)
        print("  adding " + arcname)

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
