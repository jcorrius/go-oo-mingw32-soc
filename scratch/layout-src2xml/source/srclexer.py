import sys, os.path
from globals import *
import macroparser

class EOF(Exception):
    def __init__ (self):
        pass

    def str (self):
        return "end of file"

class BOF(Exception):
    def __init__ (self):
        pass

    def str (self):
        return "beginning of file"

def removeHeaderQuotes (orig):
    if len(orig) <= 2:
        return orig
    elif orig[0] == orig[-1] == '"':
        return orig[1:-1]
    elif orig[0] == '<' and orig[-1] == '>':
        return orig[1:-1]

    return orig


def dumpTokens (tokens, toError=False):

    scope = 0
    indent = "    "
    line = ''
    chars = ''
    
    for token in tokens:
        if token in '{<':
            if len(line) > 0:
                chars += indent*scope + line + "\n"
                line = ''
            chars += indent*scope + token + "\n"
            scope += 1

        elif token in '}>':
            if len(line) > 0:
                chars += indent*scope + line + "\n"
                line = ''
            scope -= 1
            chars += indent*scope + token

        elif token == ';':
            if len(line) > 0:
                chars += indent*scope + line + ";\n"
                line = ''
            else:
                chars += ";\n"
        elif len(token) > 0:
            line += token + ' '

    if len(line) > 0:
        chars += line
    chars += "\n"
    if toError:
        sys.stderr.write(chars)
    else:
        sys.stdout.write(chars)

class SrcLexer(object):
    """Lexicographical analyzer for .src format.

The role of a lexer is to parse the source file and break it into 
appropriate tokens.  Such tokens are later passed to a parser to
build the syntax tree.
"""

    def __init__ (self, chars):
        self.parentLexer = None
        self.chars = chars
        self.bufsize = len(self.chars)

        # Properties that can be copied.
        self.debug = False
        self.debugMacro = False
        self.includeDirs = {}
        self.expandHeaders = True
        self.stopOnHeader = False

    def copyProperties (self, other):
        """Copy properties from another instance of SrcLexer."""

        self.debug = other.debug
        self.debugMacro = other.debugMacro
        self.includeDirs = other.includeDirs
        self.expandHeaders = other.expandHeaders
        self.stopOnHeader = other.stopOnHeader

    def init (self):
        self.firstNonBlank = ''
        self.token = ''
        self.tokens = []
        self.defines = {}

    def getTokens (self):
        return self.tokens

    def getDefines (self):
        return self.defines

    def nextPos (self, i):
        while True:
            i += 1
            try:
                c = self.chars[i]
            except IndexError:
                raise EOF

            if ord(c) in [0x0D]:
                continue
            break
        return i

    def prevPos (self, i):
        while True:
            i -= 1
            try:
                c = self.chars[i]
            except IndexError:
                raise BOF

            if ord(c) in [0x0D]:
                continue
            break
        return i

    def tokenize (self):
        self.init()

        i = 0
        while True:
            c = self.chars[i]

            if self.firstNonBlank == '' and not c in [' ', "\n", "\t"]:
                # Store the first non-blank in a line.
                self.firstNonBlank = c

            if c == '/':
                i = self.slash(i)
            elif c == "\n":
                i = self.lineBreak(i)
            elif c == '#':
                i = self.pound(i)
            elif c == '"':
                i = self.doubleQuote(i)
            elif c in [' ', "\t"]:
                i = self.blank(i)
            elif c in ";()[]{}<>,=+-*":
                # Any outstanding single-character token.
                i = self.anyToken(i, c)
            else:
                self.token += c

            try:
                i = self.nextPos(i)
            except EOF:
                break

        if len(self.token):
            self.tokens.append(self.token)

        if self.parentLexer == None and self.debug:
            output("all defines found:\n")
            keys = self.defines.keys()
            keys.sort()
            for key in keys:
                output("  %s\n"%key)

    def dumpTokens (self, toError=False):
        dumpTokens(self.tokens, toError)


    def maybeAddToken (self):
        if len(self.token) > 0:
            self.tokens.append(self.token)
            self.token = ''


    #--------------------------------------------------------------------
    # character handlers

    def blank (self, i):
        self.maybeAddToken()
        return i


    def pound (self, i):

        if not self.firstNonBlank == '#':
            return i

        # We are in preprocessing mode.

        command, define, buf = '', '', ''
        while True:
            try:
                i = self.nextPos(i)
                c = self.chars[i]
                if c == '\\' and self.chars[self.nextPos(i)] == "\n":
                    i = self.nextPos(i)
                    continue
            except EOF:
                break

            if c == "\n":
                i = self.prevPos(i)
                break
            elif c in [' ', "\t"] and len(buf) > 0:
                if len(command) == 0:
                    command = buf
                    buf = ''
                else:
                    buf += ' '
            else:
                buf += c

        # Use another instance of lexer to tokinize the content of the 
        # preprocessor macro.

        if command == 'define':
            self.__getMacroDefine(buf)
        elif command == 'include':
            self.__getMacroInclude(buf)

        return i


    def __getMacroDefine (self, buf):

        mparser = macroparser.MacroParser(buf)
        mparser.debug = self.debugMacro
        mparser.parse()
        macro = mparser.getMacro()
        if not macro == None:
            self.defines[macro.name] = macro
        return

        pos = buf.find(' ')
        name = buf
        if pos < 0:
            name = buf
        else:
            name = buf[:pos]
            buf = buf[pos:]
        
        mclexer = SrcLexer(buf)
        mclexer.copyProperties(self)
        mclexer.parentLexer = self
        mclexer.expandHeaders = False
        mclexer.tokenize()
        tokens = mclexer.getTokens()
        if len(tokens) > 0:
            macro = Macro(name)
            macro.tokens = tokens
            self.defines[name] = macro

        if self.debug:
            error("-"*68 + "\n")
            error('#' + command + ' ' + name + "\n")
            mclexer.dumpTokens(True)
            error("end of define\n")


    def __getMacroInclude (self, buf):

        # Strip excess string if any.
        pos = buf.find(' ')
        if pos >= 0:
            buf = buf[:pos]
        headerSub = removeHeaderQuotes(buf)

        if not self.expandHeaders:
            # We don't want to expand headers.  Bail out.
            if self.debug:
                output("%s ignored\n", headerSub)
            return

        defines = {}
        headerPath = None
        for includeDir in self.includeDirs.keys():
            hpath = includeDir + '/' + headerSub
            if os.path.isfile(hpath):
                headerPath = hpath
                break

        if headerPath == None:
            error("included header file " + headerSub + " not found\n", self.stopOnHeader)
            return
        else:
            if self.debug:
                output("%s found\n"%headerPath)
            chars = open(headerPath, 'r').read()
            mclexer = SrcLexer(chars)
            mclexer.copyProperties(self)
            mclexer.parentLexer = self
            mclexer.expandHeaders = False
            mclexer.tokenize()
            headerDefines = mclexer.getDefines()
            for key in headerDefines.keys():
                defines[key] = headerDefines[key]

        if self.debug:
            output("defines found in header:\n")
            for key in defines.keys():
                output("  %s\n"%key)

        for key in defines.keys():
            self.defines[key] = defines[key]


    def slash (self, i):
        if i < self.bufsize - 1 and self.chars[i+1] == '/':
            # Parse line comment.
            line = ''
            i += 2
            while i < self.bufsize:
                c = self.chars[i]
                if ord(c) in [0x0A, 0x0D]:
                    return i - 1
                line += c
                i += 1
            self.token = ''
        elif i < self.bufsize - 1 and self.chars[i+1] == '*':
            comment = ''
            i += 2
            while i < self.bufsize:
                c = self.chars[i]
                if c == '/' and self.chars[i-1] == '*':
                    return i
                comment += c
                i += 1
        else:
            return self.anyToken(i, '/')

        return i


    def lineBreak (self, i):

        self.firstNonBlank = ''
        self.maybeAddToken()

        return i


    def doubleQuote (self, i):
        literal = ''
        i += 1
        while i < self.bufsize:
            c = self.chars[i]
            if c == '"':
                self.tokens.append('"'+literal+'"')
                break
            literal += c
            i += 1

        return i


    def anyToken (self, i, token):
        self.maybeAddToken()
        self.token = token
        self.maybeAddToken()
        return i
