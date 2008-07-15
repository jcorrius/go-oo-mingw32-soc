
import sys
import globals

class RevisionError(Exception):
    def __init__ (self, msg='RevisionError Exception'):
        self.message = msg

class Node(object):

    def __init__ (self, num, parent):
        self.num = num
        self.children = {}
        self.branchName = ''
        self.parent = parent

    def output (self, level=0):
        isRoot = (self.parent == None)
        if isRoot:
            childKeys = self.children.keys()
            childKeys.sort()
            for key in childKeys:
                self.children[key].output(level)
            return

        sys.stdout.write("  "*level)
        node = self
        revText = ''
        while node != None:
            if len(revText) == 0:
                revText = "%d"%node.num
            else:
                revText = "%d"%node.num + '.' + revText
            node = node.parent

        revText += ' ' + self.branchName
        sys.stdout.write(revText + "\n")

        childKeys = self.children.keys()
        childKeys.sort()
        for key in childKeys:
            self.children[key].output(level+1)


class RevisionTree(globals.Debuggable):
    """Manage revision tree.

CVS's revision number has the following characteristics:

    * The first number is always 1 (for root node).
    * The second number is a revision in HEAD.
    * When the 2nd to the last number is 0, that node is a branch 
      node whose number is the original revision with that 0 removed 
      e.g. if the original number is 1.34.0.4, then that's the branch 
      node for 1.34.4.
"""

    def __init__ (self):
        globals.Debuggable.__init__(self)

        self.root = Node(1, None)


    def __revisionStr2NumList (self, revision):
        numStrs = revision.split('.')
        nums = []
        for s in numStrs:
            nums.append(int(s))
        return nums

    def addSymbol (self, name, revision):
        nums = self.__revisionStr2NumList(revision)
        n = len(nums)

        isBranchName = False
        if n >= 4 and nums[-2] == 0:
            # special case for branch name
            nums2 = nums[:-2]
            nums2.append(nums[-1])
            isBranchName = True
            nums = nums2
            n -= 1

#       sys.stderr.write ("is branch name '%s' %d '%s'\n"%(revision,isBranchName,name))
        if not isBranchName:
            return

        currentNode = self.root
        for i in xrange(1, n):
            num = nums[i]
            if not currentNode.children.has_key(num):
                currentNode.children[num] = Node(num, currentNode)
            currentNode = currentNode.children[num]
            if i == n - 1:
                # last node
                currentNode.branchName = name


    def output (self):
        self.root.output()


    def getBranchName (self, revision):
        """Get the branch name for a given revision number.

The revision number must be given as a string (e.g. '1.23.2.1').
"""
        if type(revision) != type(''):
            raise ValueError

        nums = self.__revisionStr2NumList(revision)
        node = self.root
        if nums[0] != node.num:
            raise RevisionError('first number is not 1')

        for num in nums[1:]:
            if len(node.children.keys()) == 0:
                return node.branchName

            if not node.children.has_key(num):
                raise RevisionError("revision " + revision + " doesn't have any associated branch name")

            node = node.children[num]






