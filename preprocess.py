''' author: samtenka
    change: 2019-05-20
    create: 2019-05-20
    descrp: simple processing before feeding into parser
'''

def preprocess(text):
    assert '\t' not in text, "schwa forbids tab characters" 
    lines = text.split('\n')
    uncommented = [l for l in lines if not l.strip().startswith('//')]
    return ' '.join(' '.join(uncommented).split())
