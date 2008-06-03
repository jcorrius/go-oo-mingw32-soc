#!/usr/bin/env python

import sys, popen2, os.path, os
import re
import time, datetime

currentAffiliations = {
    'OPENSTEP':               'unaffiliated',
    'ab':                     'Sun',
    'abi':                    'Sun',
    'ace_dent':               'unaffiliated',
    'af':                     'Sun',
    'aklitzing':              'unaffiliated',
    'ama':                    'Sun',
    'andreschnabel':          'unaffiliated',
    'antoxu':                 'Intel',
    'armin':                  'Sun',            # Armin Theissen (former Sun Ireland)
    'as':                     'Sun',
    'aw':                     'Sun',
    'bc':                     'Sun',
    'bei':                    'Sun',
    'beppec56':               'unaffiliated',
    'bm':                     'Sun',
    'cd':                     'Sun',
    'cedricbosdo':            'unaffiliated',
    'cl':                     'Sun',
    'cloph':                  'unaffiliated',
    'clu':                    'Sun',
    'cmc':                    'RedHat',
    'cn':                     'Sun',
    'cp':                     'Sun',
    'dbo':                    'Sun',
    'dfoster':                'Sun',
    'dkeskar':                'Intel',
    'dr':                     'Sun',
    'dv':                     'Sun',
    'dvo':                    'unaffiliated',
    'ebischoff':              'unaffiliated',
    'er':                     'Sun',
    'erack':                  'Sun',
    'ericb':                  'unaffiliated',
    'fa':                     'RedHat',         # Dan Williams
    'fheckl':                 'unaffiliated',
    'flr':                    'Novell',
    'fme':                    'Sun',
    'fpe':                    'Sun',
    'fredrikh':               'unaffiliated',
    'fridrich_strba':         'Novell',
    'fs':                     'Sun',
    'gh':                     'Sun',
    'gm':                     'Sun',
    'grichter':               'MySQL',
    'hbrinkm':                'Sun',
    'hde':                    'Sun',
    'hdu':                    'Sun',
    'hjs':                    'Sun',
    'hr':                     'Sun',
    'hro':                    'Sun',
    'ih':                     'Sun',
    'iha':                    'Sun',
    'ihi':                    'Sun',
    'is':                     'Sun',
    'jakob_lechner':          'Fabalabs',
    'jbu':                    'Sun',
    'jimmac':                 'Novell',
    'jl':                     'Sun',
    'jmarmion':               'Sun',
    'jodygoldberg':           'Novell',
    'john.marmion':           'Sun',
    'jsc':                    'Sun',
    'jsi':                    'Sun',
    'jsk':                    'Sun',
    'ka':                     'Sun',
    'kaib':                   'Google',
    'kendy':                  'Novell',
    'khong':                  'Sun',
    'kohei':                  'Novell',
    'kr':                     'Sun',
    'kso':                    'Sun',
    'kz':                     'Sun',
    'laurentgodard':          'unaffiliated',
    'liutao':                 'RedFlag',
    'liyuan':                 'RedFlag',
    'lla':                    'Sun',
    'lo':                     'Sun',
    'maho':                   'unaffiliated',
    'manikandan':             'unaffiliated',
    'mav':                    'Sun',
    'mba':                    'Sun',
    'mh':                     'Sun',
    'mhu':                    'Sun',
    'mib':                    'Sun',
    'mikeleib':               'Intel',
    'mkretzschmar':           'Google',
    'mmeeks':                 'Novell',
    'mox':                    'unaffiliated',
    'msc':                    'Sun',
    'mt':                     'Sun',
    'nemeth':                 'unaffiliated',
    'nn':                     'Sun',
    'np':                     'Sun',
    'npower':                 'Novell',
    'obo':                    'Sun',
    'obr':                    'Sun',
    'oc':                     'Sun',
    'od':                     'Sun',
    'oj':                     'Sun',
    'os':                     'Sun',
    'pb':                     'Sun',
    'pdefilippis':            'unaffiliated',
    'pj':                     'RedFlag',
    'pjanik':                 'unaffiliated',
    'pl':                     'Sun',
    'pmladek':                'Novell',
    'radekdoulik':            'Novell',
    'rene':                   'unaffiliated',
    'rt':                     'Sun',
    'sb':                     'Sun',
    'sg':                     'Sun',
    'sj':                     'Sun',
    'smsm1':                  'unaffiliated',
    'sparcmoz':               'unaffiliated',
    'ssa':                    'Sun',
    'st':                     'Sun',
    'sus':                    'Sun',
    'svesik':                 'Sun',
    'sw':                     'unaffiliated',
    'tbe':                    'Sun',
    'tbo':                    'Sun',
    'thb':                    'Sun',
    'timseves':               'SIL',
    'tl':                     'Sun',
    'tml':                    'Novell',
    'tra':                    'Sun',
    'tristan':                'unaffiliated',
    'tv':                     'Sun',
    'ufi':                    'Sun',
    'va':                     'Sun',
    'vg':                     'Sun',
    'vq':                     'unaffiliated',
    'wangyumin_ccoss':        'CCOSS',
    'ydario':                 'unaffiliated'}


def getAffiliation (name, year, month):
    affil = '(unknown)'
    if currentAffiliations.has_key(name):
        affil = currentAffiliations[name]
    return affil


class Debuggable(object):

    def __init__ (self):
        self.debug = False

    def debugPrint (self, msg, abortAfter=False):
        if self.debug:
            sys.stderr.write(msg + "\n")
            if abortAfter:
                sys.exit(1)


class RCSFile(Debuggable):

    # alpha numeric letter
    alphnum = '([a-z]|[A-Z]|[0-9])'

    # regex pattern for category match
    reCategory = '^' + alphnum + '(' + alphnum + '|\ )*\:'

    reRevSeparator = '^\-+$'


    def __init__ (self, lines):
        Debuggable.__init__(self)

        self.lines = lines
        self.lineLength = len(self.lines)
        self.reset()


    def reset (self):
        self.headers = {}
        self.branchSymbols = {}
        self.commitLogs = []
        self.descError = False
        self.symbolicNamesError = False
        self.miscError = False


    def parse (self):
        self.reset()

        rePattern = re.compile(RCSFile.reCategory)
        i = 0
        while i < self.lineLength:
            line = self.lines[i].rstrip()
            res = rePattern.search(line)
            if res == None:
                # no regex match found
                i += 1
                continue

            category = res.group(0)[:-1]
            if category == 'symbolic names':
                i = self.__parseSymbolicNames(i+1)
                continue

            if category == 'description':
                i = self.__parseDescription(i+1)
                break

            self.headers[category] = line[res.end(0):].strip()
            i += 1


    def output (self):
        for key in self.headers.keys():
            print key + " -> '" + self.headers[key] + "'"

        keys = self.branchSymbols.keys()
        keys.sort()
        for key in keys:
            print key + " -> " + self.branchSymbols[key]

        for commitLog in self.commitLogs:
            print ('-'*45)
            keys = commitLog.keys()
            keys.sort()
            for key in keys:
                print (key, "->", commitLog[key])


    def isError (self):
        return self.descError or self.symbolicNamesError or self.miscError


    def __parseSymbolicNames (self, i):

        # [tab]symbol name: branch number

        while i < self.lineLength:
            line = self.__getLine(i)
            if len(line) == 0:
                break

            if ord(line[0]) != 0x09:
                # First character is not a tab.  End of symbolic names.
                return i
    
            name, number = line.split(':')
            name = name.strip()
            number = number.strip()

            self.branchSymbols[number] = name
            i += 1

        self.symbolicNamesError = True
        self.debugPrint("error parsing symbolic names", True)
        return i


    def __getLine (self, i):
        return self.lines[i].rstrip()


    def __isRevSeparator (self, i):
        line = self.__getLine(i)
        reobj = re.compile(RCSFile.reRevSeparator)
        res = reobj.match(line)
        return res != None


    def __isEndLogSeparator (self, i):
        line = self.__getLine(i)
        return line == '='*77


    def __parseDescription (self, i):
        """ Parse commit records.

A typical comment record would look like this:

  ----------------------------
  revision 1.43.38.1
  date: 2005/10/25 12:31:22;  author: jodygoldberg;  state: Exp;  lines: +8 -4
  Issue number:  20857
  
  Trying to get this patch out of my tree into a CWS.
  The full commit failed, let's try smaller chunks.
  ----------------------------

The first two lines contain auxiliary information about the commit, while the
rest of the lines contain commit message.
"""
        if not self.__isRevSeparator(i):
            self.descError = True
            self.debugPrint("revision separator expected", True)
            return i

        self.debugPrint(self.__getLine(i))
        i += 1

        while i < self.lineLength:
            self.debugPrint(self.__getLine(i))
            commitLog = {}
    
            # revision 1.43.38.1
            line = self.__getLine(i)
            if line.find('revision') != 0:
                self.descError = True
                self.debugPrint("revision number not found: '" + line + "'", True)
                return i
    
            revnum = line.split()[1].strip()
            commitLog['revision'] = revnum
    
            i += 1
            self.debugPrint(self.__getLine(i))

            line = self.__getLine(i)
            if not self.__parseDescData(line, commitLog):
                self.descError = True
                self.debugPrint("error parsing description data", True)
                return i
    
            i += 1
    
            # the rest is a commit message.
            msg = []
            while i < self.lineLength:
                self.debugPrint(self.__getLine(i))
                if self.__isRevSeparator(i):
                    break
                elif self.__isEndLogSeparator(i):
                    return i
                line = self.__getLine(i)
                msg.append(line)
                i += 1

            commitLog['message'] = msg
            self.commitLogs.append(commitLog)

            i += 1

        return i


    def __parseDescData (self, line, commitLog):

        # date: 2005/10/25 12:31:22;  author: jodygoldberg;  state: Exp;  lines: +8 -4

        rePattern = re.compile(RCSFile.reCategory)
        for segment in line.split(';'):
            segment = segment.strip()
            if len(segment) == 0:
                continue

            res = rePattern.search(segment)
            if res == None:
                self.debugPrint("category name not found: '" + segment + "'")
                return False

            category = res.group(0)[:-1]
            value = segment[res.end(0):].strip()
            if category == 'date':
                # parse & transform a string date value into a datetime object.
                try:
                    timeValue = time.strptime(value, "%Y/%m/%d %H:%M:%S")
                    dtobj = datetime.datetime(timeValue[0], timeValue[1], timeValue[2], 
                                              timeValue[3], timeValue[4], timeValue[5])
                    commitLog[category] = dtobj
                except ValueError:
                    self.debugPrint("failed to parse a date value: '" + value + "'")
                    return False

            elif category == 'lines':
                # number of lines added
                reNum = re.compile('\+[0-9]*')
                res = reNum.search(value)
                if res == None:
                    self.debugPrint("number of added lines not found")
                    return False

                added = int(res.group(0))

                # number of lines removed
                reNum = re.compile('\-[0-9]*')
                res = reNum.search(value)
                if res == None:
                    self.debugPrint("number of removed lines not found")
                    return False

                removed = abs(int(res.group(0)))

                commitLog['added']   = added
                commitLog['removed'] = removed

            else:
                commitLog[category] = value

        return True


    def writeCommitStats (self, statObj):
        """Write commit statistics to the passed CommitStats instance.

Each commit log may have the following data:

    * added - number of lines added (integer).
    * removed - number of lines removed (integer).
    * date - commit date and time (datetime object).
    * author - who made the commit (string).
    * revision - revision number (string)
    * state - state of the file ??? (string)
    * message - commit message (string list).

Each commit log is supposed to have at least the author, revision and the date
records, while some logs may not have the added/removed line information (such
as initial commits, branch nodes etc.
"""
        for log in self.commitLogs:
            # author
            if not log.has_key('author'):
                self.debugPrint("author record is absent")
                return False
            author = log['author']

            # date
            if not log.has_key('date'):
                self.debugPrint("date record is absent")
                return False
            date = log['date']

            # added
            added = 0
            if log.has_key('added'):
                added = log['added']

            # removed
            removed = 0
            if log.has_key('removed'):
                removed = log['removed']

            if added or removed:
                statObj.add(author, date, added, removed)

        return True


class CommitStats(object):

    class Author(object):
        def __init__ (self):
            self.years = {}
    
    class Year(object):
        def __init__ (self):
            self.months = {}
    
    class Month(object):
        def __init__ (self):
            self.affiliation = '(unknown)'
            self.commitCounts = 0
            self.linesAdded = 0
            self.linesRemoved = 0

    def __init__ (self):
        self.authors = {}

    def add (self, author, date, added, removed):

        # author node
        if not self.authors.has_key(author):
            self.authors[author] = CommitStats.Author()
        authorObj = self.authors[author]

        # year node
        if not authorObj.years.has_key(date.year):
            authorObj.years[date.year] = CommitStats.Year()
        yearObj = authorObj.years[date.year]

        # month node
        if not yearObj.months.has_key(date.month):
            yearObj.months[date.month] = CommitStats.Month()
        monthObj = yearObj.months[date.month]

        monthObj.affiliation = getAffiliation(author, date.year, date.month)
        monthObj.commitCounts += 1
        monthObj.linesAdded += added
        monthObj.linesRemoved += removed


class Main(object):

    def __init__ (self):
        self.stats = CommitStats()

    def main (self, args):

        filepaths = args[1:]

        for filepath in filepaths:
            if os.path.isfile(filepath):
                r = self.__openRCSFile(filepath)
                if not r:
                    sys.stderr.write("failed to parse %s\n"%filepath)
                    sys.exit(1)
            elif os.path.isdir(filepath):
                self.__parseDir(filepath)

        self.__outputReport()


    def __parseDir (self, dirpath):
        for root, dirs, files in os.walk(dirpath):
            for filename in files:
                fullpath =  root + '/' + filename
                r = self.__openRCSFile(fullpath)
                if not r:
                    sys.stderr.write("failed to parse %s\n"%fullpath)
                    sys.exit(1)



    def __openRCSFile (self, filepath):
        cmd = "rlog " + filepath
        r, w, e = popen2.popen3(cmd)
    
        obj = RCSFile(r.readlines())
        obj.parse()
#       obj.output()
        if not obj.writeCommitStats(self.stats):
            sys.stderr.write("failed to write commit stats\n")
            sys.exit(1)
    
        r.close()
        w.close()
        e.close()
    
        return not obj.isError()


    def __outputReport (self):
        authorNames = self.stats.authors.keys()
        authorNames.sort()
        print("author\tyear\tmonth\taffiliation\tcommit count\tlines added\tlines removed")
        for authorName in authorNames:
            authorObj = self.stats.authors[authorName]
            years = authorObj.years.keys()
            years.sort()
            for year in years:
                yearObj = authorObj.years[year]
                months = yearObj.months.keys()
                months.sort()
                for month in months:
                    monthObj = yearObj.months[month]
                    print("%s\t%d\t%d\t%s\t%d\t%d\t%d"%(
                        authorName, year, month,
                        monthObj.affiliation,
                        monthObj.commitCounts, 
                        monthObj.linesAdded, monthObj.linesRemoved))



if __name__ == '__main__':
    mainObj = Main()
    mainObj.main(sys.argv)
