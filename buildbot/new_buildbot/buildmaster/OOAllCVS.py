from buildbot import util
import os, os.path, re
from rfc822 import Message
from buildbot.changes import base, changes, maildirtwisted
from buildbot.changes import mail

def parseOOAllCVSmail(self, fd, prefix=None, sep="/"):
    """Parse messages sent by the 'allcvs' program
    """
    # pretty much the same as freshcvs mail, not surprising since CVS is the
    # one creating most of the text

    m = Message(fd)
    # The mail is sent from the person doing the checkin. Assume that the
    # local username is enough to identify them (this assumes a one-server
    # cvs-over-rsh environment rather than the server-dirs-shared-over-NFS
    # model)
    name, addr = m.getaddr("from")
    if not addr:
        return None # no From means this message isn't from FreshCVS
    at = addr.find("@")
    if at == -1:
        who = addr # might still be useful
    else:
        who = addr[:at]

    # we take the time of receipt as the time of checkin. Not correct (it
    # depends upon the email latency), but it avoids the out-of-order-changes
    # issue. Also syncmail doesn't give us anything better to work with,
    # unless you count pulling the v1-vs-v2 timestamp out of the diffs, which
    # would be ugly. TODO: Pulling the 'Date:' header from the mail is a
    # possibility, and email.Utils.parsedate_tz may be useful. It should be
    # configurable, however, because there are a lot of broken clocks out
    # there.
    when = util.now()
    subject = m.getheader("subject")
    # syncmail puts the repository-relative directory in the subject:
    # mprefix + "%(dir)s %(file)s,%(oldversion)s,%(newversion)s", where
    # 'mprefix' is something that could be added by a mailing list
    # manager.
    # this is the only reasonable way to determine the directory name
    space = subject.find(" ")
    if space != -1:
        directory = subject[:space]
    else:
        directory = subject
    files = []
    comments = ""
    isdir = 0
    branch = None
    lines = m.fp.readlines()

    while lines:
        line = lines.pop(0)
        #if line == "\n":
        #    break
        #if line == "Log:\n":
        #    lines.insert(0, line)
        #    break
        line = line.lstrip()
        line = line.rstrip()

        # this begs the question of
        # how we should treat masters
        m= re.match(r'Tag: cws_\w+_(\w+)', line)
        if m:
            branch= m.group(1)
        else: 
	    continue

        #thesefiles = line.split(" ")
        #for f in thesefiles:
        #    f = sep.join([directory, f])
        #    if prefix:
        #        bits = f.split(sep)
        #        if bits[0] == prefix:
        #            f = sep.join(bits[1:])
        #        else:
        #            break

        #    files.append(f)

    while lines:
        line = lines.pop(0)
        if (line == "Modified:\n" or
            line == "Added:\n" or
            line == "Removed:\n"):
            break
							
    while lines:
        line = lines.pop(0)
        if line == "\n":
	    break
	if line == "Log:\n":
	    lines.insert(0, line)
	    break
	line = line.lstrip()
	line = line.rstrip()
        
	thesefiles = line.split(" ")
	for f in thesefiles:
            f = sep.join([directory, f])
	    if prefix:
	        bits = f.split(sep)
	        if bits[0] == prefix:
	            f = sep.join(bits[1:])
	        else:
	            break
            files.append(f)
																								     

    #if not files:
    #    return None

    if not branch:
        return None

    while lines:
        line = lines.pop(0)
        if line == "Log:\n":
            break

    while lines:
        line = lines.pop(0)
        #if line.find("Directory: ") == 0:
        #    break
        #if re.search(r"^--- NEW FILE", line):
        #    break
        #if re.search(r" DELETED ---$", line):
        #    break
        comments += line
    comments = comments.rstrip() + "\n"
    change = changes.Change(who, files, comments, isdir, when=when,
                            branch=branch)
    return change

class OOMaildirSource(mail.MaildirSource):
    parser = parseOOAllCVSmail
    name = "AllCVS"
