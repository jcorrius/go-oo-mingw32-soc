#!/usr/bin/env python

import sys, popen2, os.path, os
import re
import time, datetime
import optparse

# import local modules.
sys.path.append(sys.path[0]+"/src")
import revision, globals

# all commits that were made to the following branches are ignored.
ignoredBranches = [
    'SRX643_TF_BINFILTER',
    'cws_dev300_changefileheader', 
    'cws_src680_hedaburemove01',
    'cws_src680_ooo19126',
    'cws_src680_incguards01',
    'cws_src680_pchfix02'
]

# commits on these branches, matching regexps are ignored.
ignoredPartialBranches = {
        'cws_src680_impresstables2' : 'framework/binfilter/',
        'cws_src680_impresstables2' : 'util/svtools/',
        'cws_src680_impresstables2' : 'sw/sw/',
        'cws_src680_impresstables2' : 'gsl/vcl/',
        'cws_src680_so3deadcorpses' : 'xml/xmloff/',
        'cws_src680_so3deadcorpses' : 'oi/so3/',
        'cws_src680_so3deadcorpses' : 'sc/sc/',
}

# all commmits done by the following authors are ignored
# release engineers do eg. huge license changes directly
# on HEAD, and integrate all branches themselves.
ignoredAuthors = [
    'gh', 'ihi', 'rt', 'vg']

# just examine source code, avoiding binary bits & so on
sourceExtension = {
    '.c':1, '.cc':1, '.cpp':1, '.cs':1, '.csc':1, '.cxx':1,
    '.h':1, '.hpp':1, '.hxx':1, '.idl':1, '.java':1,
    '.py':1, '.sh':1, '.y':1}

# not strictly source, or have matching source changes & strange
# abberations in some of these, ignore for now.
wilderExtensions = {
    '.patch':1, '.diff':1, '.lst':1, 'mk':1, '.jam':1, '.scp':1,
    '.css':1, '.ini':1, '.inc':1, '.xcu':1, '.xcs':1, '.xsl':1
}

# ignore large auto-generated files that people like to regularly
# check into CVS for unknown reasons.
autogenFileRegex = [
    'sw/writerfilter/source/ooxml/gperf.*\.hxx',
    'sw/writerfilter/source/ooxml/OOXMLtokens\.hxx',
    'sw/writerfilter/source/doctok/qnametostr\.cxx',
    'sw/writerfilter/source/doctok/resources\.[ch]xx',
    'sw/writerfilter/source/ooxml/OOXMLvalues\.[ch]xx',
    'sw/writerfilter/source/resourcemodel/.*tostr\.cxx',
    'sw/writerfilter/source/ooxml/OOXMLFastTokens\.hxx',
    'sw/writerfilter/source/ooxml/OOXMLresources.*\.?xx',
    'sw/writerfilter/source/ooxml/OOXMLfastresources.*\.?xx',
    'documentation/helpcontent2/helpers/hid.lst',
    'api/odk/pack/gendocu/Attic/idl_chapter_refs_oo.txt',
    'xml/oox/source/token/tokens.txt',
]

# abberations: a big patch moving across versions, getting renamed
abberationFileRegex = [
    'external/libxmlsec/xmlsec1-1.2.4.patch',
    'external/libxmlsec/xmlsec1-1.2.6.patch',
]

currentAffiliations = {
    'ab': 'Sun', # Andreas Bregas       
    'abi': 'Sun', # Andreas Bille        
    'ace_dent': 'unaffiliated', # Andrew Dent          
    'af': 'Sun', # Andre Fischer        
    'aidan': 'unaffiliated', # Aidan Butler         
    'akhva': 'Sun', # Artem Khvat
    'aklitzing': 'unaffiliated', # Andre Klitzing (2006 GSoC student)
    'ama': 'Sun', # Andreas Martens      
    'antoxu': 'Intel', # Antonio Xu           
    'armin': 'Sun', # Armin Theissen       
    'armin.theissen': 'Sun', # Armin Theissen       
    'as': 'Sun', # Andreas Schluens
    'asrail': 'BrOffice.org', # Caio Tiago Oliveira  
    'aw': 'Sun', # Armin Weiss          
    'b_michaelsen': 'Sun', # Bjorn Michaelsen     
    'bc': 'Sun', # Behrend Cornelius    
    'bei': 'Sun', # Bernd Eilers         
    'beppec56': 'unaffiliated', # Giuseppe Castagno    
    'BerryJia': 'Sun', # Berry Jia            
    'bh': 'Sun', # Bettina Haberer      
    'Bibek': 'Trees For Life', # Bibek Sahu           
    'bluedwarf': 'unaffiliated', # Takashi Nakamoto     
    'bm': 'Sun', # Bjorn Milcke         
    'bmahbod': 'unaffiliated', # Babak Mahbod         
    'bnolte': 'unaffiliated', # Bertram Nolte        
    'bustamam': 'unaffiliated', # Bustamam Harun       
    'cd': 'Sun', # Carsten Driesner     
    'ch2000liuy': 'Redflag', # YU Liu               
    'chainchen': 'Redflag', # Jinhong Chen         
    'cj': 'Sun', # Christian Jansen     
    'cl': 'Sun', # Christian Lippka   
    'cloph': 'unaffiliated', # Christian Lohmaier  
    'cmc': 'RedHat', # Caolan McNamara      
    'cn': 'Sun', # Christoph Neumann    
    'coni': 'Sun', # Rafaella Braconi     
    'cp': 'Sun', # Christof Pintaske    
    'cphennessy': 'OpenApp', # Con Hennessy         
    'cremlae': 'unaffiliated', # Omer Bar-or          
    'cs': 'ProFOSS', # Claus Sorensen       
    'Cyb': 'Trees For Life', # Christian Junker     
    'cyrillem': 'Sun', # Cyrille Moureaux     
    'davidfraser': 'translate.org.za', # David Fraser         
    'dbo': 'Sun', # Daniel Boelzle       
    'dfoster': 'Sun', # Duncan Foster        
    'dg': 'Sun', # Dirk Grobler         
    'dkeskar': 'Intel', # Dhananjay Keskar     
    'dl': 'Sun', # Dieter Loeschky      
    'doko': 'Canonical', # Matthias Klose       
    'donqg': 'Redflag', # Quangang Dong        
    'dr': 'Sun', # Daniel Rentz         
    'drbyte': 'bytebot.net', # Colin Charles        
    'dsherwin': 'Propylon', # Darragh Sherwin      
    'duyunfen': 'Redflag', # Yunfen Du            
    'dv': 'Sun', # Dirk Volzke          
    'dvo': 'unaffiliated', # Daniel Vogelheim     
    'ebischoff': 'Bureau Cornavin', # Eric Bischoff        
    'ekato': 'unaffiliated', # Etsushi Kato         
    'er': 'Sun',    # Eike Rathke          
    'erack': 'Sun', # Eike Rathke          
    'ericb': 'unaffiliated', # Eric Bachard         
    'fa': 'RedHat', # Dan Williams         
    'fangyq': 'Redflag', # Yaqiong Fang  
    'fdechelle': 'unaffiliated', # Francois Dechelle (works in api/exthome)       
    'federicomena': 'Novell', # Federico Mena-Quinter
    'fheckl': 'unaffiliated', # Florian Heckl
    'filhocf': 'BrOffice.org', # Claudio F Filho      
    'fl': 'Sun', # Frank Loehmann
    'flr': 'Novell', # Florian Reuter       
    'fma': 'Sun', # Frank Mau            
    'fme': 'Sun', # Frank Meies          
    'fne': 'Sun', # Frank Neumann        
    'fpe': 'Sun', # Frank Peters         
    'fridrich_strba': 'Novell', # Fridrich Strba       
    'fs': 'Sun', # Frank Schonheit      
    'ganaya': 'unaffiliated', # Gene Anaya           
    'Gao Peng': 'Redflag', # Peng Gao             
    'gaozemin': 'Redflag', # Zemin Gao            
    'georgez': 'unaffiliated', # George Zahopoulos    
    'gh': 'Sun', # Gregor Hartmann      
    'ghiggins': 'Sun', # Geoff Higgins      
    'gm': 'Sun', # Gerd Weiss           
    'grichter': 'MySQL', # Georg Richter        
    'grsingleton': 'pathtech.org', # G. Roderick Singleton
    'gt': 'Sun', # Gunnar Timm (last commit in 2004)
    'gyang': 'Sun', ### unkown (2002 - 2005)
    'haggai': 'Debian', # Chris Halls       
    'hbrinkm': 'Sun', # Henning Brinkmann   
    'hdu': 'Sun', # Herbert Duerr        
    'hjs': 'Sun', # Hans-Joachim Lankenau
    'hr': 'Sun', # Jens-Heiner Rechtien 
    'hro': 'Sun', # Hennes Rohling       
    'hub': 'Novell', # Hubert Figuiere      
    'icobgr': 'unaffiliated', # Hristo Hristov       
    'ih': 'Sun', # Ilko Hoepping (works on installation)
    'iha': 'Sun', # Ingrid Halama        
    'ihi': 'Sun', # Ivo Hinkelmann       
    'is': 'Sun', # Ingo Schmidt         
    'isma87': 'unaffiliated', # Ismael Merzaq        
    'ja': 'Sun', # Joost Andrae         
    'jacky23': 'Redflag', # Sheng zhao           
    'jakob_lechner': 'Fabalabs', # Jakob Lechner        
    'jayant_madavi': 'Novell', # Jayant Balraj Madavi 
    'jb': 'Sun', # Jorg Barfurth        
    'jbrunsmann': 'unaffiliated', # Jorg Brunsmann       
    'jbu': 'Sun', # Jorg Budischewski    
    'jcn': 'Novell', # Jan Nieuwenhuizen    
    'jiamingfei': 'IBM', # Mingfei Jia          
    'jiangc': 'Redflag', # Chuang Jiang         
    'jimmac': 'Novell', # Jacob Steiner
    'jj': 'Sun', # Jorg Jahnke          
    'jl': 'Sun', # Joachim Lingner      
    'jmarmion': 'Sun', # John Marmion         
    'jmeng': 'Sun', ### unknown (2002 - 2005)
    'jobin': 'CollabNet', # Jobin Thomas
    'john.marmion': 'Sun', # John Marmion         
    'jnavrati': 'RedHat', # Jan Navratil         
    'jodygoldberg': 'Novell', # Jody Goldberg  
    'jp': 'Sun', # Juergen Pingel (last commit in 2002)      
    'jpryor': 'Novell', # Jonathan Pryor       
    'jsc': 'Sun', # Jurgen Schmidt       
    'jspindler': 'unaffiliated', # Jorg Spindler        
    'jza': 'unaffiliated', # Alexandro Colorado   
    'ka': 'Sun', # Kai Ahrens           
    'kaib': 'Google', # Kai Backman          
    'kangjingchuan': 'Redflag', # Jingchuan Kang       
    'kcarr': 'Progbits', # Scott Carr           
    'kendy': 'Novell', # Jan Holesovsky       
    'khendricks': 'unaffiliated', # Kevin Hendricks      
    'khirano': 'unaffiliated', # Hirano Kazunari      
    'khong': 'Sun', # Karl Hong            
    'kohei': 'Novell', # Kohei Yoshida        
    'kr': 'Sun', # Kay Ramme            
    'kso': 'Sun', # Kai Sommerfeld       
    'kstribley': 'unaffiliated', # Keith Stribley
    'kz': 'Sun', # Kurt Zenker          
    'larsbehr': 'Sun', # Lars Behrmann (AODL library, toolkit)
    'laurentgodard': 'inDesko/Nuxeo', # Laurent Godard       
    'lh': 'Sun', # Lutz Hoeger          
    'liangweike': 'Redflag', # Weike Liang          
    'lijian': 'Redflag', # Jian Li              
    'liujl': 'Redflag', # Jianli Liu           
    'liutao': 'Redflag', # LiuTao               
    'liuyuhua': 'Redflag', # Yuhua Liu            
    'lixxing': 'IBM', # Xing Li              
    'liyuan': 'Redflag', # Yuan Li              
    'lkovacs': 'unaffiliated', # Laszlo Kovacs        
    'lla': 'Sun', # Lars Langhans        
    'lo': 'Sun', # Lars Oppermann       
    'louis': 'Sun', # Louis Suarez-Potts   
    'Luo Jingrong': 'Redflag', # Jingrong Luo         
    'lvxg': 'Redflag', # Xugang Lv            
    'lvyue': 'Redflag', # Yue Lv               
    'maho': 'unaffiliated', # Nakata Maho          
    'maoyonggang': 'Redflag', # Yonggang Mao         
    'mav': 'Sun', # Mikhail Voitenko     
    'maveric': 'unaffiliated', # Eric Hoch            
    'mba': 'Sun', # Mathias Bauer        
    'mbu': 'Sun', # Michael Buettner
    'mci': 'unaffiliated', # Michael Cziebalski   
    'mfe': 'Sun', # Michael Ralf Fehr
    'mh': 'Sun', # Martin Hollmichel    
    'mhu': 'Sun', # Matthias Huetsch     
    'mi': 'Sun', # Michael Honnig       
    'mib': 'Sun', # Michael Brauer       
    'mikeleib': 'Intel', # Michael Leibowitz    
    'mindyliu': 'unaffiliated', # Mindy Liu            
    'mkretzschmar': 'Google', # Martin Kretzschmar   
    'mloiseleur': 'Linagora', # Michel Loiseleur     
    'mmaher': 'unaffiliated', # Martin Maher         
    'mmeeks': 'Novell', # Michael Meeks        
    'mmi': 'unaffiliated', # Michael Mi           
    'mmp': 'Sun', # Matthias Muller-Prove
    'mnicel': 'Novell', # NicelKM              
    'mod': 'unaffiliated', # Maximilian Odendahl  
    'mox': 'unaffiliated', # Mox Soini            
    'mrauch': 'unaffiliated', # Michael Rauch  
    'msicotte': 'unaffiliated', # Michael Sicotte (Aqua port)      
    'mst': 'Sun', # Michael Stahl (2007-current)
    'mt': 'Sun', # Malte Timmermann     
    'mtg': 'unaffiliated', # Martin Gallwey       
    'muthusuba': 'unaffiliated', # Muthu Subramanian    
    'mwu': 'Sun', # Minna Wu (Sun China?)
    'nemeth': 'unaffiliated',
    'nf': 'Sun', # Nils Fuhrmann        
    'nick': 'unaffiliated', # Nick Blievers        
    'nn': 'Sun', # Niklas Nebel         
    'np': 'Sun', # Nikolai Pretzell     
    'npower': 'Novell', # Noel Power           
    'obo': 'Sun', # Oliver Bolte         
    'obr': 'Sun', # Oliver Braun         
    'od': 'Sun', # Oliver Dusterhoff    
    'oj': 'Sun', # Ocke Janssen         
    'OPENSTEP': 'unaffiliated', # Edward Peterlin      
    'os': 'Sun', # Oliver Specht        
    'pagalmes': 'StarXpert', # Pierre-Andre Galmes  
    'pb': 'Sun', # Peter Burow          
    'pdefilippis': 'unaffiliated', # Pierre de Filippis   
    'pereriksson': 'unaffiliated', # Per Eriksson         
    'pflin': 'Novell', # Fong Lin             
    'pj': 'Redflag', # Peter Junge          
    'pjanik': 'unaffiliated', # Pavel Janik          
    'pjunck': 'Sun', # Pascal Junck         
    'pl': 'Sun', # Philipp Lohmann      
    'pliao': 'unaffiliated', # Ping Liao            
    'plipli': 'unaffiliated', # Sebastien Plisson    
    'pluby': 'unaffiliated', # Patrick Luby         
    'pmadhav': 'Intel', # Prasad Madhav        
    'pmladek': 'Novell', # Petr Mladek          
    'quch': 'Redflag', # Canghua Qu           
    'radekdoulik': 'Novell', # Radek Doulik         
    'rail': 'Infra-Resource', # Rail Aliev           
    'rajeshsola': 'NOSIP', # Rajesh Sola          
    'rene': 'Debian', # Rene Engelhard       
    'Rescue/k0fcc': 'Canonical', # Joey Stanford        
    'rkinsella': 'Sun', # Robert Kinsella      
    'rodarvus': 'INdT', # Rodrigo Parra Novo   
    'rpiterman': 'unaffiliated', # Ron Piterman         
    'rsiddhartha': 'Novell', # Raul Siddhartha      
    'rt': 'Sun', # Rudiger Timm         
    'rvojta': 'unaffiliated', # Robert Vojta         
    'sab': 'Sun', # Sascha Ballach       
    'sb': 'Sun', # Stephan Bergmann     
    'schmidtm': 'Sun', # Matthias Schmidt     
    'sewardj': 'unaffiliated', # Julian Seward        
    'sg': 'Sun', # Steffen Grund        
    'sgauti': 'unaffiliated', # Sophie Gautier       
    'shilei': 'Redflag', # Lei shi              
    'shiwg': 'IBM', # Wei Guo SHI          
    'shizhoubo': 'Redflag', # Zhoubo Shi           
    'sj': 'Sun', # Sven Jacobi          
    'sjanki': 'unaffiliated', # Sunil Amitkumar Janki
    'smmathews': 'unaffiliated', # Shane M Mathews      
    'smsm1': 'unaffiliated', # Shaun McDonald       
    'sparcmoz': 'clug.org.au', # Jim Watson           
    'ssa': 'Sun', # Stephan Schaefer      
    'ssmith': 'unaffiliated', # Sarah Smith          
    'st': 'Sun', # Stefan Taxhet        
    'sts': 'Sun', # Stella Schulze       
    'sus': 'Sun', # Svante Schubert (2000-current)
    'svesik': 'Sun', # Sander Vesik         
    'sw': 'unaffiliated', # Stephan Wunderlich   
    'tbe': 'Sun', # Thomas Benisch       
    'th': 'Sun', # Thomas Hosemann
    'thb': 'Novell', # Thorsten Behrens     
    'tietjens': 'unaffiliated', # Jan Tietjens         
    'timseves': 'SIL', # Tim Seves
    'tkr': 'Sun', # Tobias Krause        
    'tl': 'Sun', # Thomas Lange         
    'tml': 'Novell', # Tor Lillqvist        
    'tmorgner': 'Pentaho', # Thomas Morgner (Pentaho reporting engine)
    'toconnor': 'unaffiliated', # Tomas O'Connor       
    'tonn': 'unaffiliated', # Gerhard Tonn         
    'tonygalmiche': 'unaffiliated', # Tony Galmiche        
    'tpf': 'Sun', # Thomas Pfohe
    'tqfa': 'Redflag', # Quanfa Tang          
    'tra': 'Sun', # Tino Rachui          
    'tv': 'Sun', # Tom Verbeek          
    'ufi': 'Sun', # Uwe Fischer          
    'us': 'Sun', # Ulf Stroehler        
    'va': 'Sun', # Volker Ahrendt       
    'vg': 'Sun', # Vladimir Glazounov   
    'volody': 'unaffiliated', # Volodymyr Khrystynych
    'vq': 'Gravity Waves', # Volker Quetschke  
    'wangyumin_ccoss': 'CCOSS',   
    'waratah': 'slug.org.au', # Ken Foskey           
    'weiz': 'Redflag', # Zhao Wei             
    'willem.vandorp': 'unaffiliated', # Willem van Dorp      
    'windly': 'unaffiliated', # Wind Li              
    'wlach': 'Net Integration Technologies', # Will Lachance        
    'wuy': 'Redflag', # Yan Wu               
    'xudehua': 'Redflag', # Dehua Xu             
    'xxjack12xx': 'unaffiliated', # Jackson Low          
    'xzcheng': 'Redflag', # Xiuzhi Cheng         
    'ydario': 'Serenity Systems intl', # Yuri Dario           
    'zhanghuajun': 'Redflag', # Huajun Zhang         
    'Zhangxiaofei': 'Redflag', # Xiaofei Zhang        
    'zhaojianwei': 'Redflag', # Jianwei Zhao         
    'zhiming': 'Intel'} # Jeremy Zheng         


def getAffiliation (name, date):
    affil = '(unknown)'
    if currentAffiliations.has_key(name):
        affil = currentAffiliations[name]

    # Process names whose affiliations have changed over time.

    if name == 'thb':
        # Thorsten joined Novell in Feb 2008.
        dateJoinedNovell = datetime.datetime(2008, 2, 1)
        if date < dateJoinedNovell:
            affil = 'Sun'
        else:
            affil = 'Novell'

    elif name == 'flr':
        # Florian joined Novell in Nov 2006.
        dateJoinedNovell = datetime.datetime(2006, 11, 1)
        if date < dateJoinedNovell:
            affil = 'Sun'
        else:
            affil = 'Novell'

    elif name == 'npower':
        # Noel joined Novell in July of 2005.
        dateJoinedNovell = datetime.datetime(2005, 7, 1)
        if date < dateJoinedNovell:
            affil = 'Sun'
        else:
            affil = 'Novell'

    elif name == 'fridrich_strba':
        dateJoinedNovell = datetime.datetime(2007, 3, 1)
        if date < dateJoinedNovell:
            affil = 'unaffiliated'
        else:
            affil = 'Novell'

    elif name == 'kohei':
        dateJoinedNovell = datetime.datetime(2007, 3, 1)
        if date < dateJoinedNovell:
            affil = 'unaffiliated'
        else:
            affil = 'Novell'

    elif name == 'cmc':
        # Caolan moved from Sun to RedHat about March of 2004.
        dateJoinedRH = datetime.datetime(2004, 3, 1)
        if date < dateJoinedRH:
            affil = 'Sun'
        else:
            affil = 'RedHat'

    return affil




class RCSFile(globals.Debuggable):

    # alpha numeric letter
    alphnum = '([a-z]|[A-Z]|[0-9])'

    # regex pattern for category match
    reCategory = '^' + alphnum + '(' + alphnum + '|\ )*\:'

    # regex pattern for revision separator
    reRevSeparator = '^\-{28}$'


    def __init__ (self, lines, ext, filepath):
        globals.Debuggable.__init__(self)

        self.lines = lines
        self.lineCount = len(self.lines)
        self.ext = ext
        self.filepath = filepath;
        self.reset()


    def reset (self):
        self.headers = {}
        self.revTree = revision.RevisionTree()
        self.commitLogs = []
        self.descError = False
        self.symbolicNamesError = False
        self.miscError = False

    def parse (self):
        self.reset()

        rePattern = re.compile(RCSFile.reCategory)
        i = 0
        while i < self.lineCount:
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


    def outputRevTree (self):
        self.revTree.output()

    def getBranchName (self, revision):
        return self.revTree.getBranchName(revision)

    def output (self):
        for key in self.headers.keys():
            print key + " -> '" + self.headers[key] + "'"

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

        while i < self.lineCount:
            line = self.__getLine(i)
            if len(line) == 0:
                break

            if ord(line[0]) != 0x09:
                # First character is not a tab.  End of symbolic names.
                return i
    
            name, number = line.split(':')
            name = name.strip()
            number = number.strip()

            self.revTree.addSymbol(name, number)
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
        if self.__isEndLogSeparator(i):
            # The description block is empty.  This happens when the file is
            # committed initially without any subsequent commits.
            return i

        if not self.__isRevSeparator(i):
            self.descError = True
            self.debugPrint("revision separator expected", True)
            return i

        self.debugPrint(self.__getLine(i))
        i += 1

        while i < self.lineCount:
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
            try:
                branch = self.revTree.getBranchName(revnum)
                commitLog['branch'] = branch
            except revision.RevisionError:
                pass

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
            while i < self.lineCount:
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

                # correct for separate $Revision and $Date changes in every commit
                if added > 2:
                    added -= 2
                else:
                    added = 0
                if removed > 2:
                    removed -= 2
                else:
                    removed = 0
                    
                commitLog['added']   = added
                commitLog['removed'] = removed

            else:
                commitLog[category] = value

        return True


    def __isResyncCommit (self, msglines):
        reResync = re.compile('^RESYNC:.*;')
        for msgline in msglines:
            res = reResync.search(msgline)
            if res != None:
                return True
        return False

    def __isCwsIntegrationCommit (self, msglines):
        reCwsIntegration = re.compile('^INTEGRATION:\ CWS')
        for msgline in msglines:
            res = reCwsIntegration.search(msgline)
            if res != None:
                return True
        return False

    def writeCommitStats (self, statObj, filePath):
        """Write commit statistics to the passed CommitStats instance.

Each commit log may have the following data:

    * added - number of lines added (integer).
    * removed - number of lines removed (integer).
    * date - commit date and time (datetime object).
    * author - who made the commit (string).
    * revision - revision number (string)
    * branch - name of the branch to which the commit was made (string).
    * state - state of the file ??? (string)
    * message - commit message (string list).

Each commit log is supposed to have at least the author, revision and the date
records, while some logs may not have the added/removed line information (such
as initial commits, branch nodes etc.

Also, disregard commits whose message contains RESYNC or INTEGRATION: CWS.
"""

        issueNum = 'i[1-9][0-9]+'
        reIssueNumbers = []
#       reIssueNumbers.append(re.compile('^#' + issueNum + '#'))
#       reIssueNumbers.append(re.compile('^Issue number:\ *' + issueNum))
        reIssueNumbers.append(re.compile(issueNum))

        for log in self.commitLogs:

            statObj.totalCommitCount += 1

            if log.has_key('message'):

                # Check the message and disregard RESYNC commits.
                if self.__isResyncCommit(log['message']):
                    statObj.resyncCommitCount += 1
                    continue

                # Skip if this is a CWS integration commit.
                if self.__isCwsIntegrationCommit(log['message']):
                    statObj.integrationCommitCount += 1
                    continue

                # Check the message to see if this is a patch submission.
                isIssueNumber = False
                for msgline in log['message']:
                    for reIssueNumber in reIssueNumbers:
                        res = reIssueNumber.search(msgline)
                        if res != None:
                            txt = msgline[res.start(0):res.end(0)]
                            isIssueNumber = True
                            break

                    if isIssueNumber:
                        break

                if isIssueNumber:
                    statObj.patchCommitCount += 1

            if log.has_key('branch'):
                branch = log['branch']
            else:
                branch = ''
            # by branch
            if branch in ignoredBranches:
                self.debugPrint ("commit made to branch %s is ignored (%s)"%(branch, log['revision']))
                statObj.ignoredByBranchCount += 1
                continue
            # by branch & regexp
            if branch in ignoredPartialBranches and re.search (ignoredPartialBranches[branch], filePath):
                self.debugPrint ("commit made to partial branch %s is ignored (%s) for file %s"%(branch, log['revision'], filePath))
                statObj.ignoredByBranchCount += 1
                continue

            # author
            if not log.has_key('author'):
                self.debugPrint("author record is absent")
                return False
            author = log['author']
            if author in ignoredAuthors:
                self.debugPrint("commit made by %s is ignored"%author)
                statObj.ignoredByAuthorCount += 1
                continue


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

            if (added > 2500 or removed > 2500):
                sys.stdout.write ("huge commit: +%d -%d author %s date %s branch %s file %s\n" % (added, removed, author, date, branch, self.filepath))

            if added or removed:
                statObj.add(author, date, self.ext, added, removed, branch, self.filepath)

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
            self.extensions = {}

    class Extension(object):
        def __init__ (self):
            self.affiliation = '(unknown)'
            self.commitCounts = 0
            self.linesAdded = 0
            self.linesRemoved = 0
            self.warned = 0;

    def __init__ (self):
        self.authors = {}
        self.totalFileCount = 0
        self.totalCommitCount = 0
        self.resyncCommitCount = 0
        self.integrationCommitCount = 0
        self.ignoredByBranchCount = 0
        self.ignoredByAuthorCount = 0
        self.patchCommitCount = 0

    def add (self, author, date, ext, added, removed, branch, filePath):

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

        # extension node
        if not monthObj.extensions.has_key(ext):
            monthObj.extensions[ext] = CommitStats.Extension()
        extObj = monthObj.extensions[ext]

        extObj.affiliation = getAffiliation(author, date)
        extObj.commitCounts += 1
        extObj.linesAdded += added
        extObj.linesRemoved += removed
        if extObj.warned < 10 and extObj.linesAdded > 50000:
            extObj.warned += 1
            sys.stdout.write ("large author commit count: %d author %s date %s branch %s file %s\n" \
                                  % (extObj.linesAdded, author, date, branch, filePath))
            

class Main(object):

    def __init__ (self):
        self.stats = CommitStats()
        self.debug = False
        self.verbose = False

        self.isError = False

    def main (self):

        optparser = optparse.OptionParser()
        optparser.usage += "  file1, file2, ..."

        helptext = """specify a file that contains a list of directories to walk.
Each line in the file must correspond to each directory path.  If a directory
path is relative, it is relative to the current directory."""
        optparser.add_option('', '--dir-list', 
                             action="store", type="string", dest='dirlist',
                             help=helptext, metavar='FILE')

        helptext = """output debug messages to stderr."""
        optparser.add_option('-d', '--debug',
                             action="store_true", dest="debug",
                             help=helptext)

        helptext = """set verbose mode."""
        optparser.add_option('-v', '--verbose',
                             action="store_true", dest="verbose",
                             help=helptext)

        helptext = """specify output file to write result to."""
        optparser.add_option('-o', '--output-file',
                             action='store', type='string', dest='outputfile',
                             help=helptext, metavar='FILE')

        options, args = optparser.parse_args()

        self.debug = options.debug
        self.verbose = options.verbose

        outfile = options.outputfile

        # Check to make sure I have rlog.
        r, w, e = popen2.popen3("/usr/bin/which rlog")
        if len(r.read()) == 0:
            sys.stderr.write("rlog command not available.  You need to install rcs.")
            sys.exit(1)

        r.close()
        w.close()
        e.close()

        if options.dirlist != None:
            # directory list exists.
            self.__useDirectoryList(options.dirlist)

        filepaths = args

        for filepath in filepaths:
            if os.path.isfile(filepath):
                r = self.__openRCSFile(filepath)
                if not r:
                    sys.stderr.write("failed to parse %s\n"%filepath)
                    sys.exit(1)
            elif os.path.isdir(filepath):
                self.__parseDir(filepath)

        fd = sys.stdout
        if outfile != None and not os.path.isdir(outfile):
            fd = open(outfile, 'w')

        self.__outputReport(fd)


    def __useDirectoryList (self, filepath):
        if len(filepath) > 0 and filepath[0] == '~':
            filepath = os.environ['HOME'] + filepath[1:]

        filepath = os.path.abspath(filepath)
        if not os.path.isfile(filepath):
            sys.stderr.write("%s is not a file\n"%filepath)
            sys.exit(1)

        for dirpath in open(filepath).readlines():
            dirpath = dirpath.strip()
            if len(dirpath) == 0:
                continue
            dirpath = os.path.abspath(dirpath)
            if not os.path.isdir(dirpath):
                continue
            self.__parseDir(dirpath)


    def __parseDir (self, dirpath):
        if self.verbose:
            print("parsing directory %s"%dirpath)

        for root, dirs, files in os.walk(dirpath):
            for filename in files:
                fullpath =  root + '/' + filename
                r = self.__openRCSFile(fullpath)
                if not r:
                    sys.stderr.write("failed to parse %s\n"%fullpath)
                    sys.exit(1)


    def __getExtension (self, filepath):
        if filepath[-2:] != ',v':
            # this isn't a right RCS file name.
            sys.stderr.write("This is not an RCS file: %s\n"%filepath)
            sys.exit(1)
        filepath = filepath[:-2]
        ext = os.path.splitext(filepath)[1]
        return ext


    def __openRCSFile (self, filepath):

        if filepath[-2:] != ',v':
            # this isn't a right RCS file name.  Skip it.
            if self.verbose:
                sys.stdout.write("Skipping a non-RCS file: %s\n"%filepath)
            return True

        no_attic_path = re.subn ("/Attic/", "/", filepath)[0]
        for autogp in autogenFileRegex:
            if re.search (autogp, no_attic_path) != None:
                if self.verbose:
                    sys.stdout.write("Skipping auto-generated file: %s\n"%filepath)
                return True

        extn = self.__getExtension(filepath)
        if not sourceExtension.has_key(extn):
            if self.verbose:
                sys.stdout.write("Skipping a non-source file: %s\n"%filepath)
            return True

        cmd = "rlog " + filepath
        r, w, e = popen2.popen3(cmd)
        lines = r.readlines()
        r.close()
        w.close()
        e.close()

        obj = RCSFile(lines, extn, filepath);
        obj.debug = self.debug
        obj.parse()
        if obj.isError():
            sys.stderr.write("error parsing " + filepath + "\n")
            self.isError = True

#       obj.outputRevTree()
#       obj.output()

        if not obj.writeCommitStats(self.stats, filepath):
            sys.stderr.write("failed to write commit stats\n")
            sys.exit(1)
    

        self.stats.totalFileCount += 1

        return not obj.isError()


    def __outputReport (self, fd):
        authorNames = self.stats.authors.keys()
        authorNames.sort()
        fd.write("author\tyear\tmonth\taffiliation\text\tcommit count\tlines added\tlines removed\tdate\n")
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
                    extensions = monthObj.extensions.keys()
                    extensions.sort()
                    for ext in extensions:
                        extObj = monthObj.extensions[ext]
                        fd.write("%s\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d-%d-1\n"%(
                            authorName, year, month,
                            extObj.affiliation, ext,
                            extObj.commitCounts, 
                            extObj.linesAdded, extObj.linesRemoved,
                            year, month))

        fd.write("\n")
        fd.write("total file count\t%d\n"%self.stats.totalFileCount)
        fd.write("total commit count\t%d\n"%self.stats.totalCommitCount)
        fd.write("cws integration commits ignored\t%d\n"%self.stats.integrationCommitCount)
        fd.write("resync commits ignored\t%d\n"%self.stats.resyncCommitCount)
        fd.write("commits ignored by branch name\t%d\n"%self.stats.ignoredByBranchCount)
        fd.write("commits ignored by author name\t%d\n"%self.stats.ignoredByAuthorCount)
        fd.write("issue numbers found\t%d\n"%self.stats.patchCommitCount)



if __name__ == '__main__':
    mainObj = Main()
    mainObj.main()
