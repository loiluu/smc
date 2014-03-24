__author__ = 'secnus'
reserved = {
    'strlen'    : 'LENGTH',
    'strcasestr': 'STRCASESTR',
    'strcmp'    : 'STRCMP',
    'strcasecmp': 'STRCASECMP',
    'strstr'    : 'STRSTR',
    'match'     : 'MATCH',
    'strcat'    : 'STRCAT',
    'strcpy'    : 'STRCPY',
    'NULL'      : 'NULL',
    'query'     : 'QUERY',
}

tokens = list(reserved.values()) + [
             'VAR', 'DECNUMBER', 'NORMSTRING', "REGEX",
             'LPAREN', 'RPAREN', 'COMMA', 'OR', 'AND', "NOT",
             'EQL', 'LT', 'LTE', 'GT', 'GTE', 'NEQL',
         ]

# Tokens
t_NOT = r'!'
t_EQL = r'='
t_COMMA = r','
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_ignore = " \t"
t_LT = r'<'
t_GT = r'>'
t_LTE = r'<='
t_GTE = r'>='
t_NEQL = r'!='
t_OR = r'\|\|'
t_AND = r'&&'
t_REGEX = r'/([^"/\\"]|(\\.))*/'
t_NORMSTRING = r'\"([^\\\n]|(\\.))*?\"'

def t_VAR(t):
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    t.type = reserved.get(t.value, 'VAR')    # Check for reserved words
    return t


def t_DECNUMBER(t):
    r'\d+'
    try:
        t.value = int(t.value)
    except ValueError:
        print("Integer value too large %d", t.value)
        t.value = 0
    return t


def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

import ply.lex as lex
lex.lex()
