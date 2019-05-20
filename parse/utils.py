ANSI={
    'BLUE':'\033[34m',
    'RED':'\033[31m',
    'YELLOW':'\033[33m',
    'WHITE':'\033[37m',
}
def color_print(string, color='WHITE'):
    for k in ANSI.keys():
        string = string.replace('(%s)'%k, ANSI[k])
    print(ANSI[color] + string)
