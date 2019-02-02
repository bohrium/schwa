''' author: samuel
    create: 2019-02-01
    change: 2019-02-01
    descrp: Given backus-naur grammar and string, return parse tree.
'''

class ParseTree:
    def __init__(self, label='ROOT', kids=[]):
        self.label = label
        self.kids = kids
    def get_source(self):
        return ''.join(k if type(k)==type('') else k.get_source() for k in self.kids)

class Text:
    def __init__(self, string):
        self.string = string
        self.index = 0
    def __get_item__(self, offset):  
        return self.string[self.index + offset] 
    def peek(self):
        return self.string[self.index]
    def match_until(self, delim=' '):
        new_index = self.string.find(delim, self.index)
        if new_index==-1: new_index = len(self.string)
        word = self.string[self.index : new_index]
        self.index = new_index
        return word
    def match(self, word): 
        if self.string[self.index : self.index+len(word)] == word:
            self.index += len(word)
            return True
        return False
    def is_at_end(self):
        return self.index == len(self.string)

class ParserGenerator:
    def __init__(self, specs):
        self.rules_by_symbol = {}
        for r in filter(None, specs.split('\n')):
            split_index = r.find('=') 
            symbol, rule = r[:split_index].strip(), r[split_index+1:].strip()
            self.rules_by_symbol[symbol] = rule

        print(self.rules_by_symbol)

        self.parsers = {s:(lambda text: False) for s in self.rules_by_symbol.keys()} 
        self.parsers['EMPTY'] = lambda text: [""]
        self.get_parser = lambda symbol: ( lambda text: self.parsers[symbol](text) )

        for symbol, rule in self.rules_by_symbol.items():
            self.parsers[symbol] = self.build_labeled(symbol, self.parser_from_disjunction(Text(rule)))

    def parser_from_disjunction(self, rule):
        subparsers = []
        while not rule.is_at_end():
            subparsers.append(self.parser_from_sequence(rule))
            while rule.match(' '): pass
            if not rule.match('|'): break
                
        return self.build_disjunction(subparsers)

    def parser_from_sequence(self, rule):
        subparsers = []
        while not rule.is_at_end():
            while rule.match(' '): pass

            if rule.peek() == '(': 
                rule.match('(')
                subparsers.append(self.parser_from_disjunction(rule))
                while rule.match(' '): pass
                assert rule.match(')')
            elif rule.peek() == '"':
                rule.match('"')
                subparsers.append(self.build_literal(rule.match_until('"')))
                assert rule.match('"')
            elif rule.peek().isalpha():
                symbol = rule.match_until(' ')
                subparsers.append(self.get_parser(symbol))
            else:
                break 

        return self.build_sequence(subparsers) 

    def build_labeled(self, label, subparser):
        def labeledp(text):
            kids = subparser(text)
            if not kids: return False
            return ParseTree(label, kids) 
        return labeledp
    def build_disjunction(self, subparsers):
        def disjunctionp(text):
            for sp in subparsers:
                kids = sp(text) 
                if kids: return kids
            return False
        return disjunctionp
    def build_sequence(self, subparsers): 
        def sequencep(text):
            kids = []  
            for sp in subparsers :
                new_kids = sp(text) 
                if not new_kids: return False
                if type(new_kids) != type([]): new_kids = [new_kids]
                kids += new_kids
            return kids
        return sequencep
    def build_literal(self, literal): 
        def literalp(text):
            if text.match(literal):
                return [literal]
            return False
        return literalp 


grammar = """
    EXPRESSION = TERM ( "+" TERM | EMPTY )
    TERM = FACTOR ( "*" FACTOR | EMPTY ) 
    FACTOR = IDENTIFIER | "(" EXPRESSION ")"
    IDENTIFIER = ALPHA ( IDENTIFIER | EMPTY )  
    ALPHA = "foo" | "bar"
"""

grammar = """
    EXPRESSION = TERM ( "+" TERM | EMPTY )
    TERM = FACTOR ( "*" FACTOR | EMPTY ) 
    FACTOR = IDENTIFIER | "(" EXPRESSION ")"
    IDENTIFIER = ALPHA ( IDENTIFIER | EMPTY )  
    ALPHA = "foo" | "bar"
"""


PG = ParserGenerator(grammar)
P = PG.parsers['EXPRESSION']
PT = P(Text('foofoo*(bar+bar)+bar*bar'))
print(PT)
print(PT.get_source())
