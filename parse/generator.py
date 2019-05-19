''' author: samuel
    change: 2019-05-18
    create: 2019-05-18
    descrp: Translate Schwa to C
'''

from parser import ParseTree, Text, ParserGenerator

with open('schwa_grammar.txt') as f:
    grammar = f.read()
schwa_parser = ParserGenerator(grammar).parsers['MAIN']

ANSI={
    'BLUE':'\033[34m',
    'RED':'\033[31m',
    'YELLOW':'\033[33m',
    'WHITE':'\033[37m',
}

class Generator(object):
    def __init__(self, text):
        self.functions = {
            'schwa': {
                'argtypes_by_name': [],
                'outtype': 'int',
                'lines': []
            }
        }

        text = ' '.join(text.split())
        tree = schwa_parser(Text(text))

        self.analyze_block(tree, context='schwa')
        self.total_print()

    def total_print(self):
        # header
        print()
        print('#include <stdlib.h>')
        print('#include <stdio.h>')
        print('#define ABORT exit(1)')
        print('#define true 1')
        print('#define false 0')

        # declarations
        print()
        for context,data in self.functions.items():
            cname = context.replace(':', '_0_')
            print('%s %s(%s);' % (
                data['outtype'],
                cname,
                ', '.join('%s %s'%(tpnm, iden) for (iden, tpnm) in data['argtypes_by_name'])
            ))

        # main
        print()
        print('int main()')
        print('{')
        print('    schwa();')
        print('}')

        # definitions
        print()
        for context,data in self.functions.items():
            cname = context.replace(':', '_0_')
            print('%s %s(%s)' % (
                data['outtype'],
                cname,
                ', '.join('%s %s'%(typename, identifier) for (identifier, typename) in data['argtypes_by_name'])
            ))
            print('{')
            indent = 1
            for line in self.functions[context]['lines']:
                if not line: continue
                indent -= line[0].count('}')
                print('    '*indent + line)
                indent += line.count('{')
                indent -= line[1:].count('}')
            print('}')

    def pprint(self, string, context):
        for line in string.split('\n'):
            self.functions[context]['lines'].append(line)

    def process_declaration(self, tree):
        assert tree.label == 'DECLARATION'
        judgement, = tree.relevant_kids()
        identifier, typename = judgement.relevant_kids()
        return (x.get_source() for x in (identifier, typename))

    def process_assignment(self, tree):
        assert tree.label == 'ASSIGNMENT'
        identifier, expression = tree.relevant_kids()
        return (x.get_source() for x in (identifier, expression))

    def process_guarded_sequence(self, tree):
        body, = tree.relevant_kids()
        bodycontents = list(body.relevant_kids())
        cond_cons_pairs = []
        while bodycontents:
            bodycontents = list(bodycontents[0].relevant_kids())
            cond_cons_pairs.append((bodycontents[0], bodycontents[1]))
            bodycontents = bodycontents[2:]
        return cond_cons_pairs

    def process_function(self, tree):
        assert tree.label == 'FUNCTION'
        header, body = tree.relevant_kids()
        identifier, functype = header.relevant_kids() 
        arglist, outtype = functype.relevant_kids() 
        argtypes_by_name = []
        arglist = list(arglist.relevant_kids())
        while arglist:
            judgement = arglist[0]
            argident, argtype = judgement.relevant_kids()
            argtypes_by_name.append((argident.get_source(), argtype.get_source()))
            arglist = arglist[1:]
            if arglist:
                arglist = list(arglist[0].relevant_kids())
        return (identifier.get_source(), argtypes_by_name, outtype.get_source(), body)
 
    def analyze_block(self, tree, context, inherited_types_by_name={}):
        ''' assume no function defns within
        '''
        types_by_name = {k:v for k,v in inherited_types_by_name.items()}

        node_stack = list(tree.relevant_kids())
        while node_stack:
            k, node_stack = node_stack[0], node_stack[1:]
            if k.label == 'DECLARATION':
                identifier, typename = self.process_declaration(k)
                assert (identifier not in types_by_name), 'variable %s already declared as %s!' % (identifier, types_by_name[identifier]) 
                types_by_name[identifier] = typename
                self.pprint('%s %s;' % (typename, identifier), context)
            elif k.label == 'ASSIGNMENT':
                identifier, expression = self.process_assignment(k) 
                if identifier=='return':
                    self.pprint('return %s;' % expression.strip(), context)
                else:
                    assert (identifier in types_by_name), '%s not declared!' % identifier
                    self.pprint('%s = %s;' % (identifier, expression.strip()), context)
            elif k.label == 'IF': 
                cond_cons_pairs = self.process_guarded_sequence(k)
                for i, (cond, cons) in enumerate(cond_cons_pairs): 
                    self.pprint(
                        ('if' if i==0 else '} else if') +
                        ' (%s) {' % cond.get_source().strip()
                    , context)
                    self.analyze_block(cons, context, inherited_types_by_name=types_by_name)
                self.pprint('} else {', context)
                self.pprint('ABORT;', context)
                self.pprint('}', context)
            elif k.label == 'DO': 
                cond_cons_pairs = self.process_guarded_sequence(k)
                self.pprint('while (true) {', context)
                for i, (cond, cons) in enumerate(cond_cons_pairs): 
                    self.pprint(
                        ('if' if i==0 else '} else if') +
                        ' (%s) {' % cond.get_source().strip()
                    , context)
                    self.analyze_block(cons, context, inherited_types_by_name=types_by_name)
                self.pprint('} else {', context)
                self.pprint('break;', context)
                self.pprint('}', context)
                self.pprint('}', context)
            elif k.label == 'FUNCTION':
                identifier, argtypes_by_name, outtype, body = self.process_function(k)
                new_context = '%s:%s' % (context, identifier)
                assert (new_context not in self.functions), 'function %s already declared!'
                #print(ANSI['RED'] + 'create context %s' % new_context)
                self.functions[new_context] = {
                        'argtypes_by_name':argtypes_by_name,
                        'outtype': outtype,
                        'lines':[]
                }
                self.analyze_block(
                        body,
                        inherited_types_by_name={k:v for k,v in argtypes_by_name},
                        context=new_context
                )
            else:
                node_stack = list(k.relevant_kids()) + node_stack

   
if __name__ == '__main__':
    with open('short_program.txt') as f:
        text = f.read()
    Generator(text) 
