''' author: samuel
    change: 2019-05-18
    create: 2019-05-18
    descrp: Translate Schwa to C
'''

from parser import ParseTree, Text, ParserGenerator
import sys
assert len(sys.argv)==3
schwa_filenm, c_filenm = sys.argv[1:]

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
                'lines': [],
                'cname': 'schwa',
            }
        }

        text = ' '.join(text.split())
        tree = schwa_parser(Text(text))
        self.analyze_block(tree, context='schwa')
        print(ANSI['WHITE'] + 'successful analysis!')

    def render_header(self):
        ccode = ''
        ccode +=        '#include <stdlib.h>'
        ccode += '\n' + '#include <stdio.h>'
        ccode += '\n' + '#define ABORT exit(1)'
        ccode += '\n' + '#define true 1'
        ccode += '\n' + '#define false 0'
        ccode += '\n'
        return ccode

    def render_declarations(self):
        ccode = ''
        for context,data in self.functions.items():
            ccode += '\n' + '%s %s(%s);' % (
                data['outtype'],
                data['cname'],
                ', '.join('%s %s'%(tpnm, iden) for (iden, tpnm) in data['argtypes_by_name'])
            )
        ccode += '\n'
        return ccode

    def render_main(self):
        ccode = ''
        ccode += '\n' + 'int main()'
        ccode += '\n' + '{'
        ccode += '\n' + '    printf("\\033[33m");'
        ccode += '\n' + '    schwa();'
        ccode += '\n' + '    printf("\\033[37m");'
        ccode += '\n' + '}'
        ccode += '\n'
        return ccode

    def render_definitions(self):
        ccode = ''
        for context,data in self.functions.items():
            ccode += '\n' + '%s %s(%s)' % (
                data['outtype'],
                data['cname'],
                ', '.join('%s %s'%(typename, identifier) for (identifier, typename) in data['argtypes_by_name'])
            )
            ccode += '\n' + '{'
            indent = 1
            for line in self.functions[context]['lines']:
                if not line: continue
                indent -= line[0].count('}')
                ccode += '\n' + '    '*indent + line
                indent += line.count('{')
                indent -= line[1:].count('}')
            ccode += '\n' + '}'
            ccode += '\n'
        return ccode

    def total_print(self):
        ccode = ''
        ccode += self.render_header()
        ccode += self.render_declarations()
        ccode += self.render_main()
        ccode += self.render_definitions()
        return ccode

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
        return identifier.get_source(), expression

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
 
    def translate_expr(self, tree, context, inherited_types_by_name={}):
        ccode = ''
        for k in tree.kids:
            if type(k)==type(''):
                if not k.strip(): continue
                if k.strip() in {'and', 'or', 'not'}:
                    ccode += {'and':'&&', 'or':'||', 'not':'!'}[k.strip()]
                else:
                    ccode += k
            elif k.label == 'IDENTIFIERLOOP':
                identifier = k.get_source()
                if identifier in self.functions:
                    ccode += self.functions[identifier]['cname']
                else:
                    assert identifier in inherited_types_by_name, '%s not declared! (context %s)' % (identifier, context)
                    ccode += identifier
            else:
                ccode += self.translate_expr(k, context, inherited_types_by_name)
        if tree.label == 'LOGICFACTOR':
            ccode = '(%s)' % ccode
        return ccode

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
                    self.pprint('return %s;' % self.translate_expr(expression, context, types_by_name), context)
                else:
                    assert (identifier in types_by_name), '%s not declared! (context %s)' % (identifier, context)
                    self.pprint('%s = %s;' % (identifier, self.translate_expr(expression, context, types_by_name)), context)
            elif k.label == 'IF': 
                cond_cons_pairs = self.process_guarded_sequence(k)
                for i, (cond, cons) in enumerate(cond_cons_pairs): 
                    self.pprint(
                        ('if' if i==0 else '} else if') +
                        ' (%s) {' % self.translate_expr(cond, context, types_by_name).strip()
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
                        ' (%s) {' % self.translate_expr(cond, context, types_by_name).strip()
                    , context)
                    self.analyze_block(cons, context, inherited_types_by_name=types_by_name)
                self.pprint('} else {', context)
                self.pprint('break;', context)
                self.pprint('}', context)
                self.pprint('}', context)
            elif k.label == 'FUNCTION':
                identifier, argtypes_by_name, outtype, body = self.process_function(k)
                new_context = identifier
                assert (new_context not in self.functions), 'function %s already declared!'
                print(ANSI['RED'] + 'create context %s' % new_context)
                k
                self.functions[identifier] = {
                        'argtypes_by_name':argtypes_by_name,
                        'outtype': outtype,
                        'lines':[],
                        'cname': identifier 
                }
                self.analyze_block(
                        body,
                        inherited_types_by_name={k:v for k,v in argtypes_by_name},
                        context=new_context
                )
            elif k.label == 'PRINT': 
                identifier, = k.relevant_kids()
                identifier = identifier.get_source()
                assert (identifier in types_by_name), '%s not declared! (print context %s)' % (identifier, context)
                typename = types_by_name[identifier]
                if typename == 'float':
                    self.pprint('printf("%s \t %%f\\n", %s);' % (identifier, identifier), context)
                elif typename == 'int':
                    self.pprint('printf("%s \t %%d\\n", %s);' % (identifier, identifier), context)
                elif typename == 'bool':
                    self.pprint('printf("%s \t %%s\\n", %s?"true":"false");' % (identifier, identifier), context)
                else:
                    assert False
            else:
                node_stack = list(k.relevant_kids()) + node_stack

   
if __name__ == '__main__':
    with open(schwa_filenm) as f:
        text = f.read()
    G = Generator(text) 
    with open(c_filenm, 'w') as f:
        f.write(G.total_print())
