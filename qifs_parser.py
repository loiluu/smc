__author__ = 'secnus'
import ply.yacc as yacc
from qifs_lexer import tokens
from core import *
from utils import *
from sympy.logic.boolalg import Or, And, Not, to_cnf
from sympy.core.symbol import Symbol
import sys
#dictionary of names
names = {}
noConstraint = 0
listvar = {}
query_var = None
query_len = 0
stats = {'concat' : 0, 'regex':0, 'contains' : 0, 'len' : 0, 'compare' : 0, 'strstr':0, 'others' : 0}
precedence = (
    ('left', 'OR'),
    ('left', 'AND'),
    ('right', 'NOT'),
)
def p_formula_1(t):
    'formula : LPAREN formula RPAREN'
    t[0] = t[2]

def p_formula_2(t):
    'formula : constraint'
    t[0] = FormSingle(t[1])


def p_formula_3(t):
    'formula : NOT formula'
    t[0] = FormNeg(t[2])

def p_formula_4(t):
    'formula : formula AND formula'
    t[0] = FormAnd(t[1], t[3])

def p_formula_5(t):
    'formula : formula OR formula'
    t[0] = FormOr(t[1], t[3])

def p_constraint(t):
    '''constraint : concat
                  | length
                  | contains
                  | strstr
                  | equal
                  | regex
                  | allstring
            '''
    t[0] = t[1]


def p_concat(t):
    'concat : var EQL var CONCAT var'
    global stats
    stats['concat'] += 1
    t[0] = ConcatCons(t[1], t[3], t[5])


def p_equal(t):
    'equal : var EQL var'
    t[0] = EqualCons(t[1], t[3])
    global stats
    stats['others'] += 1

def p_equal2(t):
    'equal : NORMSTRING EQL var'
    global stats
    stats['compare'] += 1
    t[0] = EqualCons2(t[3], t[1].replace(" ", "a"))

def p_equal3(t):
    'equal : var EQL NORMSTRING'
    global stats
    stats['compare'] += 1
    t[0] = EqualCons2(t[1], t[3].replace(" ", "a"))

def p_contains(t):
    'contains : var CONTAINS NORMSTRING'
    global stats
    stats['contains'] += 1    
    t[0] = ContainCons(t[1], t[3])

def p_strstr(t):
    'strstr : var STRSTR NORMSTRING DECNUMBER'
    t[0] = StrstrCons(t[1], t[3], t[4])
    global stats
    stats['strstr'] += 1

def p_length(t):
    '''length : LENGTH LPAREN var RPAREN LT DECNUMBER
              | LENGTH LPAREN var RPAREN GT DECNUMBER
              | LENGTH LPAREN var RPAREN LTE DECNUMBER
              | LENGTH LPAREN var RPAREN GTE DECNUMBER
              | LENGTH LPAREN var RPAREN EQL DECNUMBER'''
    global stats
    stats['len'] += 1
    t[0] = LenCons(t[3], t[5], t[6])

def p_regex(t):
    'regex : var IN REGEX'
    t[0] = RegCons(t[1], t[3])
    global stats
    stats['regex'] += 1

def p_allstring(t):
    'allstring : ALLSTRING var'
    t[0] = allString(t[1])
    global stats
    stats['others'] += 1

def p_var(t):
    'var : VAR'
    t[0] = t[1]
    if not (t[1] in listvar):
        listvar[t[1]] = []
    
def p_error(t):
    print("Syntax error at '%s'" % t)

#Convert constraint expression to sympy.logic expression
def tag_constraints(f):
    if isinstance(f, Constraint):
        global noConstraint
        noConstraint+=1
        names['C'+str(noConstraint)] = f
        return 'C'+str(noConstraint)
    else:
        if isinstance(f, FormSingle):
            return tag_constraints(f.value)
        elif isinstance(f, FormAnd):
            sleft = tag_constraints(f.left)
            sright = tag_constraints(f.right)
            return sleft + "&" + sright
        elif isinstance(f, FormOr):
            sleft = tag_constraints(f.left)
            sright = tag_constraints(f.right)
            return '(' + sleft + "|" + sright + ')'
        elif isinstance(f, FormNeg):
            return '~' + tag_constraints(f.value)

#convert sympy.logic cnf expression to intermediate form
#The expression will be in form [ex1, ex2, [OrExp1, OrExp2, ..], ex3,ex4]
def convert_to_cnf_list(cnfex):
    rtlist = []
    if type(cnfex) == And:
        for iAnd in cnfex.args:
            if type(iAnd) == Or:
                sublist = []
                for iOr in iAnd.args:
                    if type(iOr) == Not:
                        id = iOr.args[0]
                        constr = names[str(id)]
                        sublist.append(FormNeg(FormSingle(constr)))
                    elif type(iOr) == Symbol:
                        constr = names[str(iOr)]
                        sublist.append(FormSingle(constr))
                rtlist.append(sublist)
            elif type(iAnd) == Not:
                id = iAnd.args[0]
                constr = names[str(id)]
                rtlist.append(FormNeg(FormSingle(constr)))
            elif type(iAnd) == Symbol:
                constr = names[str(iAnd)]
                rtlist.append(FormSingle(constr))

    elif type(cnfex) == Or:
        sublist = []
        for iOr in cnfex.args:
            if type(iOr) == Not:
                id = iOr.args[0]
                constr = names[str(id)]
                sublist.append(FormNeg(FormSingle(constr)))
            elif type(iOr) == Symbol:
                constr = names[str(iOr)]
                sublist.append(FormSingle(constr))
        rtlist.append(sublist)
    elif type(cnfex) == Not:
        id = cnfex.args[0]
        constr = names[str(id)]
        rtlist.append(FormNeg(FormSingle(constr)))
    elif type(cnfex) == Symbol:
        constr = names[str(cnfex)]
        rtlist.append(constr)

    return rtlist

#check if f is a constraint on var
def belong(f, var):
    if isinstance(f, FormNeg):
        return belong(f.value, var)
    
    if isinstance(f, FormSingle):        
        if f.value.var == var:
            return True
    
#check if list of expressions lista is included
# in list of expressions listb
def notin(lista, listb):
    result = True
    for ilist in listb:
        if type(ilist) == list:
            if len(ilist) != len(lista):
                continue
            iresult = False
            for i in ilist:
                if not (i in lista):
                    result = True
                    break

            for i in lista:
                if not (i in ilist):
                    result = True
                    break
            if not iresult:
                return False

    return result

#return a list of constraints on var
def getConstraintList(cnflist, var):
    rtlist = []

    for cexp in cnflist:
        if type(cexp) == list:
            sublist = []
            for iOr in cexp:
                if belong(iOr, var):
                    sublist.append(iOr)
            #have more variables in that OR, read the paper to
            #see how we handle such case
            if len(sublist) and (len(sublist) < len(cexp)):
                sublist.append(1)
            if len(sublist) and notin(sublist, rtlist):
                rtlist.append(sublist)
        else:
            if belong(cexp, var):
                if not (cexp in rtlist):
                    rtlist.append(cexp)

    return rtlist
parser = yacc.yacc(start='formula')

if __name__ == "__main__":
    input_file = "qifs_input.txt"
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    f = open(input_file)
    parse_result = None
    for line in f:
        line = line.strip()
        if len(line) == 0:
            continue
        if line.startswith("query"):
            query_var = line.split()[1]
            if len(line.split()) > 2:
                query_len = line.split()[2]        
            continue

        _result = parser.parse(line)
        if not parse_result:
            parse_result = _result
        else:
            parse_result = FormAnd(parse_result, _result)
    if isinstance(parse_result, FormSingle):
       parse_result = FormAnd(parse_result, parse_result)
    
    #convert to Conjunction Normal Form using to_cnf of sympy
    logic_expression = tag_constraints(parse_result)
    cnfex = to_cnf(logic_expression)

    #convert the cnf expression to intermediate form
    cnflist = convert_to_cnf_list(cnfex)
    
    #constraints grouping
    for var in listvar:
        listvar[var] = getConstraintList(cnflist, var)
        for (i, exp) in enumerate(listvar[var]):
            listvar[var][i] = removeDuplicateConstraint(listvar[var][i])

    #print to file in given structure
    # and call C model counting
    print_to_file(listvar, query_var, query_len)
    
    #print statistics
    sum = 0
    for u, v in stats.iteritems():
        sum += v
    print "No. of constraints: " + str(sum)
    print "No. of contains constraints: " + str(stats['contains'])
    print "No. of concat constraints: " + str(stats['concat'])
    print "No. of regex constraints: " + str(stats['regex'])
    print "No. of comparisons to const string: " + str(stats['compare'])
    print "No. of length operators: " + str(stats['len'])
    print "No. of strstr operators: " + str(stats['strstr'])
    print "No. of other operators: " + str(stats['others'])
