''' author: samuel
    change: 2019-05-20
    create: 2019-05-18
    descrp: translate schwa to C
'''

from parser import ParseTree, Text, ParserGenerator
from preprocess import preprocess
from utils import color_print

with open('templates/main_template.c') as f: main_template= f.read()
with open('templates/nn_globals_template.c') as f: nn_globals_template = f.read()
with open('templates/weight_init_template.c') as f: weight_init_template = f.read()
with open('templates/forward_template.c') as f: forward_template = f.read()
with open('templates/log_in_history_template.c') as f: log_in_history_template= f.read()
with open('templates/backward_template.c') as f: backward_template = f.read()
with open('templates/update_baseline_template.c') as f: update_baseline_template = f.read()

class Generator(object):
    def __init__(self, parse_tree):
        self.functions = {
            'main': {
                'argtypes_by_name': [],
                'outtype': 'int',
                'lines': [],
                'cname': '_main',
            },
            'uniform': {
                'argtypes_by_name': [],
                'outtype': 'float',
                'lines': ['return uniform();'],
                'cname': '_uniform',
            },
            'laplace': {
                'argtypes_by_name': [],
                'outtype': 'float',
                'lines': ['return laplace();'],
                'cname': '_laplace',
            },
        }
        self.switch_data = {
            #0: {
            #   'nb_inputs':    3 
            #   'nb_hidden':    4
            #   'nb_outputs':   2
            #},
        }

        self.analyze_block(parse_tree, context='main')
        #print(parse_tree.display())
        color_print('successful analysis!', color='WHITE')

    def render_func_declarations(self):
        ccode = ''
        for context,data in self.functions.items():
            ccode += '\n' + '%s %s(%s);' % (
                data['outtype'],
                data['cname'],
                ', '.join('%s %s'%(tpnm, iden) for (iden, tpnm) in data['argtypes_by_name'])
            )
        ccode += '\n'
        return ccode

    def render_nn_globals(self):
        ccode = ''
        for s_index, d in self.switch_data.items():
            I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
            ccode += (nn_globals_template 
                .replace('/*S_INDEX*/'   , str(s_index))
                .replace('/*NB_INPUTS*/' , str(I))
                .replace('/*NB_HIDDEN*/' , str(H))
                .replace('/*NB_OUTPUTS*/', str(O))
            )
        return ccode

    def render_weight_init(self):
        ccode = ''
        for s_index, d in self.switch_data.items():
            I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
            ccode += (weight_init_template
                .replace('/*S_INDEX*/'   , str(s_index))
                .replace('/*NB_INPUTS*/' , str(I))
                .replace('/*NB_HIDDEN*/' , str(H))
                .replace('/*NB_OUTPUTS*/', str(O))
            )
        return ccode

    def render_forward(self, s_index):
        d = self.switch_data[s_index]
        I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
        ccode = (forward_template
            .replace('/*S_INDEX*/'   , str(s_index))
            .replace('/*NB_INPUTS*/' , str(I))
            .replace('/*NB_HIDDEN*/' , str(H))
            .replace('/*NB_OUTPUTS*/', str(O))
        )
        return ccode

    def render_log_in_history(self, s_index):
        d = self.switch_data[s_index]
        I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
        ccode = (log_in_history_template
            .replace('/*S_INDEX*/'   , str(s_index))
            .replace('/*NB_INPUTS*/' , str(I))
        )
        return ccode

    def render_backward(self, s_index):
        d = self.switch_data[s_index]
        I, H, O = d['nb_inputs'], d['nb_hidden'], d['nb_outputs']
        ccode = (backward_template
            .replace('/*S_INDEX*/'   , str(s_index))
            .replace('/*NB_INPUTS*/' , str(I))
            .replace('/*NB_HIDDEN*/' , str(H))
            .replace('/*NB_OUTPUTS*/', str(O))
            .replace('/*FORWARD_PASS*/', self.render_forward(s_index))
        )
        return ccode

    def render_update_baseline(self):
        ccode = update_baseline_template
        return ccode



    def render_func_implementations(self):
        ccode = ''
        for context,data in self.functions.items():
            ccode += '\n' + '%s %s(%s)' % (
                data['outtype'],
                data['cname'],
                ', '.join('%s _%s'%(typename, identifier) for (identifier, typename) in data['argtypes_by_name'])
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
        ccode = (main_template 
                .replace('/*LEARNING_RATE*/'                , str(0.01))
                .replace('/*RANDOM_SEED*/'                  , str(1729))
                .replace('/*WEIGHT_INIT_SCALE*/'            , str(0.1))
                .replace('/*WEIGHT_CLIP_SCALE*/'            , str(5.0))
                .replace('/*HISTORY_CAPACITY*/'             , str(64))
                .replace('/*BASELINE_AVG_TIMESCALE*/'       , str(100))
                .replace('/*LRELU_LEAK*/'                   , str(0.2))
                .replace('/*WEIGHT_INIT*/'                  , self.render_weight_init())
                .replace('/*NN_GLOBALS*/'                   , self.render_nn_globals())
                .replace('/*USER_FUNCTION_DECLARATIONS*/'   , self.render_func_declarations())
                .replace('/*USER_FUNCTION_IMPLEMENTATIONS*/', self.render_func_implementations())
        )
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
                    ccode += '_'+identifier
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
                self.pprint('%s _%s;' % (typename, identifier), context)
            elif k.label == 'ASSIGNMENT':
                identifier, expression = self.process_assignment(k) 
                if identifier=='return':
                    self.pprint('return %s;' % self.translate_expr(expression, context, types_by_name), context)
                else:
                    assert (identifier in types_by_name), '%s not declared! (context %s)' % (identifier, context)
                    self.pprint('_%s = %s;' % (identifier, self.translate_expr(expression, context, types_by_name)), context)
            elif k.label == 'IF': 
                cond_cons_pairs = self.process_guarded_sequence(k)
                assert cond_cons_pairs, 'alternative constructs must have at least one branch'
                for i, (cond, cons) in enumerate(cond_cons_pairs): 
                    self.pprint(
                        ('if' if i==0 else '} else if') +
                        ' (%s) {' % self.translate_expr(cond, context, types_by_name).strip()
                    , context)
                    self.analyze_block(cons, context, inherited_types_by_name=types_by_name)
                self.pprint('} else {', context)
                self.pprint('printf("FAILED ALTERNATIVE CONSTRUCT (%s etc)\\n");' % cond_cons_pairs[0][0].get_source(), context)
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
                assert cons_list, 'switch constructs must have at least one branch'
                s_index = len(self.switch_data) 
                color_print('create switch %d...' % s_index, color='RED')
                self.switch_data[s_index] = {
                        'nb_inputs': len(arglist)+1,
                        'nb_hidden': len(arglist)*len(cons_list),
                        'nb_outputs': len(cons_list),
                }
                for i,x in enumerate(arglist):
                    self.pprint('input%d[%d] = _%s;' % (s_index, i, x), context) 
                self.pprint('input%d[%d] = 1.0;' % (s_index, len(arglist)), context)
                for line in self.render_forward(s_index).split('\n'):
                    self.pprint(line.strip(), context)
                for line in self.render_log_in_history(s_index).split('\n'):
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
                color_print('create context %s...' % new_context, color='RED')
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
                expression, = k.relevant_kids()
                color_print('create reward %s...' % expression.get_source(), color='RED')
                self.pprint('reward = %s;' % self.translate_expr(expression, context, types_by_name), context)
                for s_index in self.switch_data.keys():
                    for line in self.render_backward(s_index).split('\n'):
                        self.pprint(line.strip(), context)
                for line in self.render_update_baseline().split('\n'):
                    self.pprint(line.strip(), context)

            elif k.label == 'PRINT': 
                identifier, = k.relevant_kids()
                identifier = identifier.get_source()
                assert (identifier in types_by_name), '%s not declared! (print context %s)' % (identifier, context)
                typename = types_by_name[identifier]
                if typename == 'float':
                    self.pprint('printf("%s \\t %%f\\n", _%s);' % (identifier, identifier), context)
                elif typename == 'int':
                    self.pprint('printf("%s \\t %%d\\n", _%s);' % (identifier, identifier), context)
                elif typename == 'bool':
                    self.pprint('printf("%s \\t %%s\\n", _%s?"true":"false");' % (identifier, identifier), context)
                else:
                    assert False
            else:
                node_stack = list(k.relevant_kids()) + node_stack

   
if __name__ == '__main__':
    import sys
    if len(sys.argv) == 1:
        schwa_filenm, c_filenm, verbose = 'toy_programs/compare.schwa', 'compiled/compare.c', False 
    elif len(sys.argv) == 3:
        schwa_filenm, c_filenm = sys.argv[1:3]
    else:
        assert len(sys.argv) in [1, 3], "expect 0 or 2 command line arguments"

    schwa_parser = ParserGenerator('grammars/schwa.grammar').parsers['MAIN']
    with open(schwa_filenm) as f:
        text = f.read()
    text = preprocess(text)
    tree = schwa_parser(Text(text))

    G = Generator(tree) 
    with open(c_filenm, 'w') as f:
        f.write(G.total_print())
