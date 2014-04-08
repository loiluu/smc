__author__ = 'secnus'
import ply.yacc as yacc
from raw_to_smc_lexer import tokens
from core import *
from rawutils import *

#dictionary of new names
names = {}
no_var = 0

def gen_new_var():
    global no_var
    no_var += 1
    return "var_" + str(no_var)

def p_formula_1(t):
    'formula : LPAREN formula RPAREN'
    t[0] = t[2]

def p_formula_2(t):
    'formula : constraint'
    t[0] = t[1]


def p_formula_3(t):
    'formula : NOT formula'
    t[0] = "!" + (t[2])


def p_formula_4(t):
    'formula : formula AND formula'
    t[0] = t[1] + ' and ' + t[3]

def p_formula_5(t):
    'formula : formula OR formula'
    t[0] = "(" + t[1] + " or " + t[3] + ")"


def p_constraint(t):
    '''constraint : concat
                  | length
                  | strcmp
                  | strstr
                  | strcpy
                  | regex
                  | mark_query
            '''
    t[0] = t[1]


def p_concat(t):
    'concat : STRCAT LPAREN var COMMA var RPAREN'
    new_name = gen_new_var()
    t[0] = new_name + '=' + t[3] + '.' + t[5]
    names[t[3]] = new_name

def p_concat2(t):
    'concat : STRCAT LPAREN var COMMA cstring RPAREN'
    new_name1 = gen_new_var()
    new_name2 = gen_new_var()
    t[0] = new_name1 + '=' + t[3] + '.' + new_name2 + ' and ' + new_name2 + ' = \"' + t[5] + '\"'
    names[t[3]] = new_name1


def p_strcpy(t):
    'strcpy : STRCPY LPAREN var COMMA var RPAREN'
    t[0] = t[3] + '=' + t[5]

def p_strcpy2(t):
    'strcpy : STRCPY LPAREN var COMMA cstring RPAREN'
    t[0] = t[3] + ' = \"' + t[5] + '\"'

def p_regex(t):
    'regex : MATCH LPAREN var COMMA REGEX RPAREN'
    t[0] = t[3] + ' in ' + t[5]

def p_strstr(t):
    'strstr : STRSTR LPAREN var COMMA cstring RPAREN EQL DECNUMBER'
    t[0] = t[3] + " strstr " + " \"" + t[5] + "\" " + t[8]

def p_strstr1(t):
    'strstr : STRSTR LPAREN var COMMA cstring RPAREN EQL NULL'
    t[0] = "!" + t[3] + " contains " + " \"" + t[5] + "\" "

def p_strstr2(t):
    'strstr : STRSTR LPAREN var COMMA cstring RPAREN NEQL NULL'
    t[0] = t[3] + " contains " + " \"" + t[5] + "\" "

def p_strstr3(t):
    'strstr : STRSTR LPAREN cstring COMMA var RPAREN EQL NULL'
    string_list = get_all_sub_strings_original(t[3])
    result = ""
    for i_string in string_list:
        result += "!" + t[5] + "=\"" + i_string + "\"\n"
    t[0] = result

def p_strstr4(t):
    'strstr : STRSTR LPAREN cstring COMMA var RPAREN NEQL NULL'
    string_list = get_all_sub_strings_original(t[3])
    result = "("
    for i_string in string_list:
        result += t[5] + "=\"" + i_string + "\" or "
    t[0] = result[:len(result)-3] + ')'

def p_strstr5(t):
    'strstr : STRCASESTR LPAREN cstring COMMA var RPAREN EQL NULL'
    string_list = get_all_sub_strings_insensitive(t[3])
    result = ""
    for i_string in string_list:
        result += "!" + t[5] + "=\"" + i_string + "\"\n"
    t[0] = result

def p_strstr6(t):
    'strstr : STRCASESTR LPAREN cstring COMMA var RPAREN NEQL NULL'
    string_list = get_all_sub_strings_insensitive(t[3])
    result = "("
    for i_string in string_list:
        result += t[5] + "=\"" + i_string + "\" or "
    t[0] = result[:len(result)-3] + ')'

def p_strstr7(t):
    'strstr : STRCASESTR LPAREN var COMMA cstring RPAREN NEQL NULL'
    result = "("
    list_strings = get_all_case_strings(t[5])
    for i_string in list_strings:
        if len(result) == 1:
            result += t[3] + " contains " + " \"" + i_string + "\" "
        else:
            result += " or " + t[3] + " contains " + " \"" + i_string + "\" "
    t[0] = result + ')'

def p_strstr8(t):
    'strstr : STRCASESTR LPAREN var COMMA cstring RPAREN EQL NULL'
    result = ""
    list_strings = get_all_case_strings(t[5])
    for i_string in list_strings:
        if len(result) == 0:
            result += "!" + t[3] + " contains " + " \"" + i_string + "\" "
        else:
            result += " and !" + t[3] + " contains " + " \"" + i_string + "\" "
    t[0] = result

def p_length(t):
    '''length : LENGTH LPAREN var RPAREN LT DECNUMBER
              | LENGTH LPAREN var RPAREN GT DECNUMBER
              | LENGTH LPAREN var RPAREN LTE DECNUMBER
              | LENGTH LPAREN var RPAREN GTE DECNUMBER
              | LENGTH LPAREN var RPAREN EQL DECNUMBER'''
    t[0] = 'len (' + t[3] + ')' + t[5] + str(t[6])


def p_strcmp(t):
    'strcmp : STRCMP LPAREN var COMMA cstring RPAREN'
    t[0] = '!' + t[3] + " =  " + " \"" + t[5] + "\""


def p_strcmp2(t):
    'strcmp : STRCASECMP LPAREN var COMMA cstring RPAREN'
    t[0] = "!" + t[3] + " = " + "\"" + get_case_regex_not_null(t[5]) + "\" "

def p_mark_query(t):
    'mark_query : QUERY var'
    t[0] = t[1] + " " + t[2]

def p_var(t):
    'var : VAR'
    tmp = t[1]
    while tmp in names:
        tmp = names[tmp]
    if t[1] in names:
        names[t[1]] = tmp
    t[0] = tmp


def p_cstring(t):
    'cstring : NORMSTRING'
    t[0] = t[1][:len(t[1])-1][1:]


def p_error(t):
    print("Syntax error at '%s'" % t)

parser = yacc.yacc(start='formula')

f = open("rawinput.txt", "r")
result = ""
for line in f:
    if len(line.strip()):
        parse_result = parser.parse(line)
        if len(result):
            result += "\n" + parse_result
        else:
            result = parse_result

g = open("p_input.txt", "w")
g.write(result)
