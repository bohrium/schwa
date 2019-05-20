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
        self.switch_data = {
            #0: {
            #   'nb_inputs':    3 
            #   'nb_hidden':    4
            #   'nb_outputs':   2
            #},
        }

        text = ' '.join(text.split())
        tree = schwa_parser(Text(text))
        #tree.display()
        self.analyze_block(tree, context='schwa')
        print(ANSI['WHITE'] + 'successful analysis!')

    def render_header(self):
        ccode = ''
        ccode +=        '#include <stdlib.h>'
        ccode += '\n' + '#include <stdio.h>'
        ccode += '\n' + '#include <math.h>'
        ccode += '\n' + '#define ABORT exit(1)'
        ccode += '\n' + '#define bool char'
        ccode += '\n' + '#define true 1'
        ccode += '\n' + '#define false 0'
        ccode += '\n' + 'static int i, j;'
        ccode += '\n' + 'static float r, cumulative;'
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

    def render_nns(self):
        ccode = ''
        for k, d in self.switch_data.items():
            I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
            ccode += '\n' + 'static float input%d[%d];' % (k, I)
            ccode += '\n' + 'static float weight_u%d[%d][%d];' % (k, H, I)
            ccode += '\n' + 'static float active_h%d[%d];       static float dlossd_h%d[%d];' % (k, H, k, H)
            ccode += '\n' + 'static float active_z%d[%d];       static float dlossd_z%d[%d];' % (k, H, k, H)
            ccode += '\n' + 'static float weight_v%d[%d][%d];' % (k, O, H)
            ccode += '\n' + 'static float active_hh%d[%d];      static float dlossd_hh%d[%d];' % (k, O, k, O)
            ccode += '\n' + 'static float active_exphh%d[%d];' % (k, O)
            ccode += '\n' + 'static float partition%d;' % k
            ccode += '\n' + 'static int sample%d;' % k
        ccode += '\n' + 'static float reward;'
        ccode += '\n'
        return ccode

    def render_weight_initialization(self):
        ccode = ''
        ccode += '\n' + 'float uniform()'
        ccode += '\n' + '{'
        ccode += '\n' + '    return ((float)rand())/RAND_MAX;'
        ccode += '\n' + '}'
        ccode += '\n' + 'float laplace()'
        ccode += '\n' + '{'
        ccode += '\n' + '    return log(uniform()) * (rand()%2 ? -1 : +1);'
        ccode += '\n' + '}'
        ccode += '\n' + 'void initialize_weights()'
        ccode += '\n' + '{'
        for k, d in self.switch_data.items():
            I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
            ccode += '\n' + '    for (int i=0; i!=%d; ++i) {' % H
            ccode += '\n' + '        for (int j=0; j!=%d; ++j) {' % I
            ccode += '\n' + '            weight_u%d[i][j] = 0.1 * laplace();' % k
            ccode += '\n' + '        }'
            ccode += '\n' + '    }'
            ccode += '\n' + '    for (int i=0; i!=%d; ++i) {' % O
            ccode += '\n' + '        for (int j=0; j!=%d; ++j) {' % H
            ccode += '\n' + '            weight_v%d[i][j] = 0.1 * laplace();' % k
            ccode += '\n' + '        }'
            ccode += '\n' + '    }'
        ccode += '\n' + '}'
        ccode += '\n' + 'float lrelu(float h)'
        ccode += '\n' + '{'
        ccode += '\n' + '        return (h<0 ? 0.2 * h : h);'
        ccode += '\n' + '}'
        ccode += '\n' + 'float dlrelu(float h)'
        ccode += '\n' + '{'
        ccode += '\n' + '        return (h<0 ? 0.2 : 1.0);'
        ccode += '\n' + '}'
        ccode += '\n'
        return ccode

    def render_forward(self, s_index):
        ccode = ''
        ccode += '\n' + 'for (i=0; i!=4; ++i) {'
        ccode += '\n' + '    active_h%d[i] = 0.0;' % s_index
        ccode += '\n' + '    for (j=0; j!=3; ++j) {'
        ccode += '\n' + '        active_h%d[i] += weight_u%d[i][j] * input%d[j];' % (s_index, s_index, s_index)
        ccode += '\n' + '    }'
        ccode += '\n' + '    active_z%d[i] = lrelu(active_h%d[i]);' % (s_index, s_index)
        ccode += '\n' + '}'
        ccode += '\n' + 'partition%d = 0.0;' % s_index
        ccode += '\n' + 'for (i=0; i!=2; ++i) {'
        ccode += '\n' + '    active_hh%d[i] = 0.0;' % s_index
        ccode += '\n' + '    for (j=0; j!=4; ++j) {'
        ccode += '\n' + '        active_hh%d[i] += weight_v%d[i][j] * active_z%d[j];' % (s_index, s_index, s_index)
        ccode += '\n' + '    }'
        ccode += '\n' + '    active_exphh%d[i] = exp(active_hh%d[i]);' % (s_index, s_index)
        ccode += '\n' + '    partition%d += active_exphh%d[i];' % (s_index, s_index)
        ccode += '\n' + '}'
        ccode += '\n' + 'r = uniform();'
        ccode += '\n' + 'cumulative = 0.0;'
        ccode += '\n' + 'for (i=0; i!=2; ++i) {'
        ccode += '\n' + '    cumulative += active_exphh%d[i] / partition%d;' % (s_index, s_index)
        ccode += '\n' + '    if (cumulative >= r) { break; }'
        ccode += '\n' + '}'
        ccode += '\n' + 'sample%d = i;' % s_index
        ccode += '\n'
        return ccode

    def render_backward(self, s_index):
        pass
        #float g = reward - baseline;

        #int i,j;
        #for (i=0; i!=2; ++i) {
        #        dlossd_hh[i] = - g * active_exphh[i] / partition;
        #}
        #dlossd_hh[sample] += g;

        #for (j=0; j!=4; ++j) {
        #        dlossd_z[j] = 0.0;
        #        for (i=0; i!=2; ++i) {
        #                dlossd_z[j] += weight_v[i][j] * dlossd_hh[i];
        #                weight_v[i][j] += learning_rate * dlossd_hh[i] * active_z[j]; 
        #                //weight_v[i][j] = clip5(weight_v[i][j]);
        #        }
        #        dlossd_h[j] = dlossd_z[j] * dlrelu(active_h[j]); 
        #}

        #for (j=0; j!=3; ++j) {
        #        for (i=0; i!=4; ++i) {
        #                weight_u[i][j] += learning_rate * dlossd_h[i] * input[j]; 
        #                //weight_u[i][j] = clip5(weight_u[i][j]);
        #        }
        #}


    def render_main(self):
        ccode = ''
        ccode += '\n' + 'int main()'
        ccode += '\n' + '{'
        ccode += '\n' + '    initialize_weights();'
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
        ccode += self.render_nns()
        ccode += self.render_weight_initialization()
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

    def process_switch(self, tree):
        args, body, = tree.relevant_kids()

        argtup, = args.relevant_kids()
        argtupcontents, = argtup.relevant_kids()
        argtupcontents = list(argtupcontents.relevant_kids())
        arglist = []
        while argtupcontents:
            arglist.append(argtupcontents[0].get_source())
            argtupcontents = argtupcontents[1:]
            if argtupcontents:
                argtupcontents = list(argtupcontents[0].relevant_kids())

        bodycontents = list(body.relevant_kids())
        cons_list = []
        while bodycontents:
            bodycontents = list(bodycontents[0].relevant_kids())
            cons_list.append(bodycontents[0])
            bodycontents = bodycontents[1:]

        return arglist, cons_list

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
            elif k.label == 'SWITCH':
                arglist, cons_list = self.process_switch(k)
                for arg in arglist:
                    assert arg in types_by_name, 'switch arg %s unrecognized (context %s)' % (arg, context)
                s_index = len(self.switch_data) 
                self.switch_data[s_index] = {
                        'nb_inputs': len(arglist)+1,
                        'nb_hidden': len(arglist)*len(cons_list),
                        'nb_outputs': len(cons_list),
                }
                for i,x in enumerate(arglist):
                    self.pprint('input%d[%d] = %s;' % (s_index, i, x), context) 
                self.pprint('input%d[%d] = 1.0;' % (s_index, len(arglist)), context)
                for line in self.render_forward(s_index).split('\n'):
                    self.pprint(line.strip(), context)
                self.pprint('switch (sample%d) {' % s_index, context)
                for i, c in enumerate(cons_list):
                    self.pprint('case %d: {' % i, context)
                    self.analyze_block(c, context, inherited_types_by_name=types_by_name)
                    self.pprint('} break;', context)
                self.pprint('}', context)
            elif k.label == 'FUNCTION':
                identifier, argtypes_by_name, outtype, body = self.process_function(k)
                new_context = identifier
                assert (new_context not in self.functions), 'function %s already declared!'
                print(ANSI['RED'] + 'create context %s' % new_context)
                self.functions[identifier] = {
                        'argtypes_by_name':argtypes_by_name,
                        'outtype': outtype,
                        'lines':[],
                        'cname': '_' + identifier 
                }
                self.analyze_block(
                        body,
                        inherited_types_by_name={k:v for k,v in argtypes_by_name},
                        context=new_context
                )
            elif k.label == 'REWARD':
                amount, = k.relevant_kids()
                self.pprint('reward = %s;' % amount.get_source(), context)
                # TODO: add backward code
            elif k.label == 'PRINT': 
                identifier, = k.relevant_kids()
                identifier = identifier.get_source()
                assert (identifier in types_by_name), '%s not declared! (print context %s)' % (identifier, context)
                typename = types_by_name[identifier]
                if typename == 'float':
                    self.pprint('printf("%s \\t %%f\\n", %s);' % (identifier, identifier), context)
                elif typename == 'int':
                    self.pprint('printf("%s \\t %%d\\n", %s);' % (identifier, identifier), context)
                elif typename == 'bool':
                    self.pprint('printf("%s \\t %%s\\n", %s?"true":"false");' % (identifier, identifier), context)
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
