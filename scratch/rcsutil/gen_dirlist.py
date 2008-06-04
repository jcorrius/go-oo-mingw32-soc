#!/usr/bin/env python

import optparse, re, sys

def error (msg, abortAfter=False):
    sys.stderr.write(msg + "\n")
    if abortAfter:
        sys.exit(1)

def main ():
    optparser = optparse.OptionParser()
    options, args = optparser.parse_args()
    file = open('./modules', 'r')
    lines = file.readlines()
    file.close()
    parseLines(lines)

def parseLines (lines):

    # alpha numeric letter
    alphnum = '([a-z]|[A-Z]|[0-9]|_)'
    alphnumslash = '([a-z]|[A-Z]|[0-9]|_|/)'
    alias = '^' + alphnum + alphnumslash + '*\ +' + alphnum + alphnumslash + '+\ *$'

    reOOO3 = re.compile("^OpenOffice3\ \-a.*$")
    reAlias = re.compile(alias)
    OOO3modules = []
    aliases = {}
    for line in lines:
        line = line.rstrip()
        res = reOOO3.search(line)
        if res != None:
            # OpenOffice3 modules found!
            # OpenOffice3 -a module1 module2 ...
            line = line[res.start(0):res.end(0)]
            words = line.split()
            if len(words) == 0 or not words[0] == 'OpenOffice3' or not words[1] == '-a':
                error("mal-formatted OpenOffice3 modules", True)
            OOO3modules = words[2:]
            continue

        res = reAlias.search(line)
        if res != None:
            # alias found
            line = line[res.start(0):res.end(0)]
            words = line.split()
            if len(words) != 2:
                error("mal-formatted alias line: '%s'"%line, True)
            name, value = words[0], words[1]
            aliases[name] = value
            continue

    for module in OOO3modules:
        if not aliases.has_key(module):
            error("alias for %s doesn't exist"%module, True)
        dirname = aliases[module]
        print dirname

if __name__ == '__main__':
    main()
