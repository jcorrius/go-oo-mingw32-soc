
import globals


class TokenBase(object):
    def __init__ (self, tokens):
        self.tokens = tokens

    def parse (self, i):
        return i

    def getText (self):
        return ''

class Add(TokenBase): pass
class Sub(TokenBase): pass
class Mul(TokenBase): pass
class Div(TokenBase): pass
class Power(TokenBase): pass
class Concat(TokenBase): pass
class LT(TokenBase): pass
class LE(TokenBase): pass
class EQ(TokenBase): pass
class GE(TokenBase): pass
class GT(TokenBase): pass
class NE(TokenBase): pass
class Isect(TokenBase): pass
class List(TokenBase): pass
class Range(TokenBase): pass

class Plus(TokenBase): pass
class Minus(TokenBase): pass
class Percent(TokenBase): pass

class NameX(TokenBase):
    """external name"""

    def parse (self, i):
        i += 1
        self.refID = globals.getSignedInt(self.tokens[i:i+2])
        i += 2
        self.nameID = globals.getSignedInt(self.tokens[i:i+2])
        i += 2
        return i

    def getText (self):
        return "<externname externsheet ID: %d; name ID: %d>"%(self.refID, self.nameID)


tokenMap = {
    # binary operator
    0x03: Add,
    0x04: Sub,
    0x05: Mul,
    0x06: Div,
    0x07: Power,
    0x08: Concat,
    0x09: LT,
    0x0A: LE,
    0x0B: EQ,
    0x0C: GE,
    0x0D: GT,
    0x0E: NE,
    0x0F: Isect,
    0x10: List,
    0x11: Range,

    # unary operator
    0x12: Plus,
    0x13: Minus,
    0x14: Percent,

    # operand tokens
    0x39: NameX,
    0x59: NameX,
    0x79: NameX,

    # last item
    0xFF: None
}

class FormulaParser(object):
    def __init__ (self, tokens):
        self.tokens = tokens
        self.text = ''

    def parse (self):
        # first 2-bytes contain the length of the formula tokens
        length = globals.getSignedInt(self.tokens[0:2])
        if length <= 0:
            return

        ftokens = self.tokens[2:2+length]
        i = 0
        while i < length:
            tk = ftokens[i]
            if type(tk) == type('c'):
                # get the ordinal of the character.
                tk = ord(tk)
            if not tokenMap.has_key(tk):
                i += 1
                continue

            o = tokenMap[tk](ftokens)
            i = o.parse(i)
            self.text += o.getText() + ' '

            i += 1


    def getText (self):
        return self.text
