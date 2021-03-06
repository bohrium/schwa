''' author: samuel
    change: 2019-05-18
    create: 2019-02-01
    descrp: Given backus-naur grammar and string, return parse tree.
'''

from utils import color_print
from preprocess import preprocess

class ParseTree:
    ''' 
    '''
    def __init__(self, label='ROOT', kids=[], ignore=False, unroll=False):
        self.label = label
        self.kids = kids
        self.ignore = ignore 
        self.unroll = unroll 

    def get_source(self):
        return ''.join(k if type(k)==type('') else k.get_source() for k in self.kids)

    def width(self): 
        return len([k for k in self.kids if type(k)==type('') or not k.ignore])

    def relevant_kids(self):
        ''' no literal strings and no ignoreds '''
        return (k for k in self.kids if type(k)!=type('') and not k.ignore)

    def display(self, depth=0, delim='   :', collapse=False):
        '''
                `collapse` indicates whether to collapse unary vines down to leafmost node of vine 
        '''
        if self.ignore:
            return
        elif not self.unroll and (not collapse or self.width()!=1):
            source = self.get_source()
            if len(source)>64+3:
                source = source[:32] + '(WHITE)...(YELLOW)' + source[-32:]
            source = '(YELLOW)' + source + '(WHITE)'
            color_print(delim*depth + '(BLUE)' + self.label + '(WHITE)[' + source + ']')
            depth += 1
        for k in self.kids:
            if type(k)==type(''):
                color_print(delim*depth + '(RED)' + k + '(WHITE)')
            else:
                k.display(depth, delim, collapse)

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
    def __init__(self, specs_filename, verbose=False):
        with open(specs_filename) as f:
            specs = f.read()

        self.rules_by_symbol = {}
        self.ignore = set([])
        self.unroll = set([])
        for r in filter(None, specs.split('\n')):
            split_index = r.find('=') 
            symbol, rule = r[:split_index].strip(), r[split_index+1:].strip()
            if symbol[-1]=='*':
                symbol = symbol[:-1]
                self.ignore.add(symbol)
            elif symbol[-1]=='!':
                symbol = symbol[:-1]
                self.unroll.add(symbol)

            self.rules_by_symbol[symbol] = rule

        if verbose:
            print(self.rules_by_symbol)

        self.parsers = {s:(lambda text: False) for s in self.rules_by_symbol.keys()} 
        self.get_parser = lambda symbol: ( lambda text: self.parsers[symbol](text) )

        for symbol, rule in self.rules_by_symbol.items():
            self.parsers[symbol] = self.build_labeled(symbol, self.parser_from_disjunction(Text(rule)), symbol in self.ignore, symbol in self.unroll)

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

    def build_labeled(self, label, subparser, ignore, unroll):
        def labeledp(text):
            kids = subparser(text)
            if not kids: return False
            return ParseTree(label, kids, ignore, unroll) 
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
            for sp in subparsers:
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


if __name__ == '__main__':
    import sys
    
    if len(sys.argv) == 1:
        grammar_filenm, program_filenm = 'grammars/schwa.grammar', 'toy_programs/dijkstra.schwa'
    elif len(sys.argv) == 3:
        grammar_filenm, program_filenm = sys.argv[1:3]
    else:
        assert len(sys.argv) in [1, 3], "expect 0 or 2 command line arguments"

    PG = ParserGenerator(grammar_filenm)
    P = PG.parsers['MAIN']
    with open(program_filenm) as f:
        text = f.read()
    text = preprocess(text)
    color_print(text, color='WHITE')
    PT = P(Text(text))
    PT.display()
