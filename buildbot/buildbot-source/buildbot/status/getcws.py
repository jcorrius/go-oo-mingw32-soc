# Original thanks to David Fraser <davidf@sjsoft.com> and Caolan McNamara <caolanm@redhat.com>

import urllib2, cookielib, cgi
import os, sys

from HTMLParser import HTMLParser

class cws:
    def __init__(self, cwss):
        self.cwss = cwss
        

class EISScraper(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.state = 0;
	self.cwss = []
	
    def handle_starttag(self, tag, attrs):
	if tag == 'td' and self.state < 3:
            self.state += 1 
    
    def handle_data(self, data):
        if self.state == 3:
	    self.cwss.append(data.strip())
            self.state = 4
        
    
    def handle_endtag(self, tag):
	if tag == 'tr' and self.state == 4:
            self.state = 0
        
class EIS:
    def __init__(self, cookiefile="eis.lwp"):
        self.cookiefile = cookiefile
        self.cookiejar = cookielib.LWPCookieJar()
        if os.path.isfile(self.cookiefile):
          self.cookiejar.load(self.cookiefile)
        opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cookiejar))
        urllib2.install_opener(opener)
        self.login()
        self.cache = {}

    def login(self):
        urllib2.urlopen("http://eis.services.openoffice.org/EIS2/GuestLogon").read()
        self.cookiejar.save(self.cookiefile)

    def cacheurl(self, url):
        if url in self.cache:
            return self.cache[url]
        else:
            try:
                contents = urllib2.urlopen(url).read()
            except urllib2.HTTPError, e:
                if e.code == 401:
                    self.login()
                    contents = urllib2.urlopen(url).read()
                else:
                    raise
            self.cache[url] = contents
            return contents
    def findcws(self, cws,):
       thiscwsid = None
       milestoneresults = self.cacheurl("http://eis.services.openoffice.org/EIS2/cws.SearchCWS?DATE_NULL_Integrated_After=&DATE_NULL_DueDateBefore=&INT_NULL_Priority=&Name=" + cws + "&SRC_Step=Search&INT_NULL_IsHelpRelevant=&RSV_NoWait=true&DATE_NULL_DueDateAfter=&TaskId=&DATE_NULL_Integrated_Before=&INT_NULL_IsUIRelevant=")
       for line in milestoneresults.replace("\r", "").split("\n"):
            # cws.ShowCWS?Path=SRC680%2Fm54%2Fdba15&Id=1431
            startmark, endmark = "'cws.ShowCWS?", "'"
            if startmark in line:
                cwsargs = line[line.find(startmark) + len(startmark):]
                cwsargs = cwsargs[:cwsargs.find(endmark)]
                cwsargs = cgi.parse_qs(cwsargs)
                thiscwsid = int(cwsargs["Id"][0])

       return thiscwsid


    def getCWSs(self, query):
       status = -1
       if query == "new":
           status = 1     
       elif query == "nominated":
           status = 2     
       elif query == "integrated":
           status = 3
       elif query == "cancelled":
           status = 4
       elif query == "deleted":
           status = 5
       elif query == "ready":
           status = 6       
       elif query == "planned":
           status = 7
       elif query == "approved":
           status = 8
       elif query == "pre-nominated":
           status = 9
       elif query == "fixed":
           status = 10
       elif query == "finished":
           status = 11
       elif query == "cloned":
           status = 12

       cwsresults = self.cacheurl("http://eis.services.openoffice.org/EIS2/cws.SearchCWS?Status=" + `status`  +"&MWS=3&RSV_NoWait=true&SRC_Step=Search")  
       
       foo = EISScraper()
       foo.feed(cwsresults)
       foo.cwss = foo.cwss[1:]
       foo.cwss.sort(lambda x, y: cmp(x.lower(), y.lower()))
       return cws(foo.cwss)

    def getcwsid(self, cwsname):
        somecwsid = self.findcws(cwsname)
        if somecwsid != None:
            return somecwsid
        raise ValueError("no id found for cws %s" % cwsname)

    def getcwsurl(self, cwsname):
        cwsid = self.getcwsid(cwsname)
        return self.cacheurl("http://eis.services.openoffice.org/EIS2/cws.ShowCWS?Id=%d" % cwsid)



class GetCWS:
    def __init__(self, query):
        self.query = query

    def getCWSs(self):
        eis = EIS()
        info = eis.getCWSs(self.query)  
        return info.cwss


