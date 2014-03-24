__author__ = 'secnus'
reserved = {
    'len'       : 'LENGTH',
    'or'        : 'OR',
    'and'       : 'AND',
    'contains'  : 'CONTAINS',
    'strstr'    : 'STRSTR',
    'in'        : 'IN',
    'allstring' : 'ALLSTRING'
}

tokens = list(reserved.values()) + [
             'VAR', 'DECNUMBER', 'NORMSTRING', "REGEX",
             'CONCAT', 'LPAREN', 'RPAREN',
             'EQL', 'LT', 'LTE', 'GT', 'GTE', 'NOT',
         ]

# Tokens
t_REGEX = r'/([^"/\\"]|(\\.))*/'
t_NORMSTRING = r'"([^"\\]|(\\.))*"'
t_EQL = r'='
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_CONCAT = r'\.'
t_ignore = " \t"
t_LT = r'<'
t_GT = r'>'
t_LTE = r'<='
t_GTE = r'>='
t_NOT = r'!'


def t_VAR(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    t.type = reserved.get(t.value, 'VAR')    # Check for reserved words
    return t


def t_DECNUMBER(t):
    r'\-?\d+'
    try:
        t.value = min(int(t.value), 2000000)
    except ValueError:
        print("Integer value too large %d", t.value)
        t.value = 0
    return t


def t_newline(t):
    r'\n+'
    t.lexer.lineno += 1

# Error handling rule
def t_error(t):
    print "Illegal character '%s' at line '%d' " %(t.value[0], t.lexer.lineno)
    t.lexer.skip(1)

import ply.lex as lex
lex.lex()
