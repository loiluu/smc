__author__ = 'secnus'
from core import *


def get_print_format(t):
    if isinstance(t, FormNeg):
        return "1 \n" + get_print_format(t.value)
    if isinstance(t, FormSingle):
        if isinstance(t.value, ContainCons):
            return "3 " + t.value.pattern[1:len(t.value.pattern)-1]
        if isinstance(t.value, StrstrCons):
            return "4 " + t.value.pattern[1:len(t.value.pattern)-1] + " " + str(t.value.number)
        if isinstance(t.value, EqualCons):
            return "5 " + t.value.var_1
        if isinstance(t.value, ConcatCons):
            return "6 " + t.value.var_1 + " " + t.value.var_2
        if isinstance(t.value, LenCons):
            return "7 " + t.value.op + " " + str(t.value.number)
        if isinstance(t.value, RegCons):
            return "8 " + t.value.pattern[1:len(t.value.pattern)-1]
        if isinstance(t.value, EqualCons2):
            return "9 " + t.value.pattern[1:len(t.value.pattern)-1]
        if isinstance(t.value, allString):
            return "10"

#print the constraint in intermediate to file
# 0 var_name; start new variable
# 1 constraint: not constraint.
# 2 number_of_constrains upper_bound_is_g(z): for list of OR constraints (next number lines
#       contain inside constraints
# 3 pattern: contains constraint
# 4 pattern position: strstr constraint
# 5 var: equal to var
# 6 var1 var2: var1 \circ var2
# 7 op number: length operators
# 8 regexp: match regex
# 9 consStr: equal to const string
# 10 : is arbitrary string
# 11 var: is query var


def print_to_file(listvar, query_var, query_len):
    f = open('inter_input.txt', 'w')

    #remove empty strings
    listvar = deal_with_empty(listvar)
    #replace a = b if we have equal
    listvar = deal_with_equal(listvar)

    #cleanning the constraints
    for var in listvar:
        listvar[var] = removeDuplicateConstraint(listvar[var])
    for var in listvar:
        listvar[var] = removeSameEquals(listvar[var])

    listOrder = build_the_graph(listvar)

    f.write(str(len(listOrder)) + "\n")
    for var in listOrder:
        f.write("0 " + var + "\n")
        for exp in listvar[var]:
            if type(exp) == list:
                if (1 in exp):
                    line = "2 " + str(len(exp) - 1) + " 1\n"
                else:
                    line = "2 " + str(len(exp)) + " 0\n"
                f.write(line)
                for iexp in exp:
                    if iexp != 1:
                        f.write(get_print_format(iexp) + "\n")
            else:
                f.write(get_print_format(exp) + "\n")
        if query_var == var:
            f.write("11 " + str(query_len) + "\n")
    f.close()

# Build the concat (dependency) graph.
# After that, if var = var1.var2 and
# listOrder[i] = var, listOrder[j1]= var1, listOrder[j2]= var2 then
# i > j1, i > j2


def build_the_graph(listvar):
    #calucate number of variables that each variable depends on
    dependency = {}
    for var in listvar:
        dependency[var] = []
        for exp in listvar[var]:
            #if exp is the list of OR
            if type(exp) == list:
                for iexp in exp:
                    if iexp != 1:
                        if isinstance(iexp, FormNeg):
                            if isinstance(iexp.value, FormSingle):
                                if isinstance(iexp.value.value, ConcatCons):
                                    var1 = iexp.value.value.var_1
                                    var2 = iexp.value.value.var_2
                                    if not var1 in dependency[var]:
                                        dependency[var].append(var1)
                                    if not var2 in dependency[var]:
                                        dependency[var].append(var2)
                                if isinstance(iexp.value.value, EqualCons):
                                    var1 = iexp.value.value.var_1
                                    if not var1 in dependency[var]:
                                        dependency[var].append(var1)

                        elif isinstance(iexp, FormSingle):
                            if isinstance(iexp.value, ConcatCons):
                                var1 = iexp.value.var_1
                                var2 = iexp.value.var_2
                                if not var1 in dependency[var]:
                                    dependency[var].append(var1)
                                if not var2 in dependency[var]:
                                    dependency[var].append(var2)
            #else if exp is a single clause
            else:
                if isinstance(exp, FormNeg):
                    if isinstance(exp.value, FormSingle):
                        if isinstance(exp.value.value, ConcatCons):
                            var1 = exp.value.value.var_1
                            var2 = exp.value.value.var_2
                            if not var1 in dependency[var]:
                                dependency[var].append(var1)
                            if not var2 in dependency[var]:
                                dependency[var].append(var2)
                        if isinstance(exp.value.value, EqualCons):
                            var1 = exp.value.value.var_1
                            if not var1 in dependency[var]:
                                dependency[var].append(var1)
                elif isinstance(exp, FormSingle):
                    if isinstance(exp.value, ConcatCons):
                        var1 = exp.value.var_1
                        var2 = exp.value.var_2
                        if not var1 in dependency[var]:
                            dependency[var].append(var1)
                        if not var2 in dependency[var]:
                            dependency[var].append(var2)
    #get the list Order
    listOrder = []
    while True:
        found = False
        for var in dependency:
            if not (var in listOrder) and len(dependency[var]) == 0:
                found = True
                listOrder.append(var)
                for k in dependency:
                    if var in dependency[k]:
                        dependency[k].remove(var)
        if not found:
            break
    return listOrder

#replace b by a if we have a = b
#not replace a = b expression in the Or Expression
def deal_with_equal(listvar):
    while True:
        var1 = None
        for var in listvar:
            for exp in listvar[var]:
                if type(exp) != list and isinstance(exp.value, EqualCons):
                    if exp.value.var_1 != var:
                        var1 = exp.value.var_1
                        break
            if var1 != None:
                break

        if (var1 == None):
            break

        #for Kaluza input, we don't want to replace the
        #secrete value
        if var1.startswith("var_0xINPUT"):
            #replace var by var1
            for ivar in listvar:
                listvar[ivar] = replace_var(listvar[ivar], var1, var)
            listvar[var1].extend(listvar[var])
            listvar.pop(var)
        else:
            #replace var1 by var
            for ivar in listvar:
                listvar[ivar] = replace_var(listvar[ivar], var, var1)
            listvar[var].extend(listvar[var1])
            listvar.pop(var1)
    return listvar

#replace var1 by var in iexp
def replace_var_exp(iexp, var, var1):
    if isinstance(iexp.value, ConcatCons):
        var_1 = iexp.value.var_1
        var_2 = iexp.value.var_2
        if var1 == var_1:
            iexp.value.var_1 = var
        if var1 == var_2:
            iexp.value.var_2 = var
    elif isinstance(iexp.value, EqualCons):
        var_1 = iexp.value.var_1
        if var1 == var_1:
            iexp.value.var_1 = var

    if iexp.value.var == var1:
        iexp.value.var = var
    return iexp

#replace var1 by var in list of exp
def replace_var(listexp, var, var1):
    for (i, exp) in enumerate(listexp):
        if type(exp) == list:
            for (j, iexp) in enumerate(exp):
                if iexp != 1:
                    if isinstance(iexp, FormNeg):
                        if isinstance(iexp.value, FormSingle):
                            iexp.value = replace_var_exp(iexp.value, var, var1)
                            exp[j] = iexp
                    elif isinstance(iexp, FormSingle):
                        exp[j] = replace_var_exp(iexp, var, var1)
            listexp[i] = exp
        else:
            if isinstance(exp, FormNeg):
                exp.value = replace_var_exp(exp.value, var, var1)
            elif isinstance(exp, FormSingle):
                exp = replace_var_exp(exp, var, var1)
            listexp[i] = exp

    return listexp

#compare two expressions. Return true if they are equal
def similar(exp1, exp2):
    if isinstance(exp1, FormNeg) and isinstance(exp2, FormNeg):
        return similar(exp1.value, exp2.value)
    if isinstance(exp1, FormSingle) and isinstance(exp2, FormSingle):
        return similar(exp1.value, exp2.value)
    if isinstance(exp1, ConcatCons) and isinstance(exp2, ConcatCons):
        return exp1.var == exp2.var and ((exp1.var_1 == exp2.var_1 and exp1.var_2 == exp2.var_2) or
                                         (exp1.var_2 == exp2.var_1 and exp1.var_1 == exp2.var_2))

    if isinstance(exp1, ContainCons) and isinstance(exp2, ContainCons):
        return exp1.var == exp2.var and (exp1.pattern == exp2.pattern)

    if isinstance(exp1, StrstrCons) and isinstance(exp2, StrstrCons):
        return exp1.var == exp2.var and (exp1.pattern == exp2.pattern) and (exp1.number == exp2.number)

    if isinstance(exp1, RegCons) and isinstance(exp2, RegCons):
        return exp1.var == exp2.var and (exp1.pattern == exp2.pattern)

    if isinstance(exp1, LenCons) and isinstance(exp2, LenCons):
        return exp1.var == exp2.var and (exp1.op == exp2.op) and (exp1.number == exp2.number)

    if isinstance(exp1, EqualCons) and isinstance(exp2, EqualCons):
        return exp1.var == exp2.var and (exp1.var_1 == exp2.var_1)

    if isinstance(exp1, EqualCons2) and isinstance(exp2, EqualCons2):
        return exp1.var == exp2.var and (exp1.pattern == exp2.pattern)
    return False

#remove duplication in constraint list of variables
def removeDuplicateConstraint(listExp):
    if type(listExp) != list:
        return listExp
    while True:
        t = len(listExp)
        found = []
        for i in range(0, t):
            if type(listExp[i]) == list:
                continue
            for j in range(i+1, t):
                if type(listExp[j]) == list:
                    continue
                if similar(listExp[i], listExp[j]):
                    found.append(j)
                    break
            if len(found):
                break
        if not len(found):
            break
        for i in reversed(found):
            del listExp[i]
    return listExp


#after dealing with equals, we have some constraints like a = a
#this function removes those
def removeSameEquals(listExp):
    if type(listExp) != list:
        return listExp
    while True:
        t = len(listExp)
        found = []
        for i in range(0, t):
            if isinstance(listExp[i], FormSingle):
                if isinstance(listExp[i].value, EqualCons):
                    if listExp[i].value.var == listExp[i].value.var_1:
                        found.append(i)
        if not len(found):
            break
        for i in reversed(found):
            del listExp[i]
    return listExp

#check if the concat constraint has empty operand
def check_has_empty_string(constraint, list_empty):
    return constraint.var_1 in list_empty or constraint.var_2 in list_empty


#convert concat constraint to equal constraint
def replace_empty_string(constraint, list_empty):
    if constraint.var_1 in list_empty and constraint.var_2 in list_empty:
        return LenCons(constraint.var, "=", 0)
    if constraint.var_1 in list_empty:
        return EqualCons(constraint.var, constraint.var_2)
    return EqualCons(constraint.var, constraint.var_1)


#if we have constraint a = b.c && length(c) = 0 then we can transform it to a = b
def deal_with_empty(listvar):
    list_empty = []
    for var in listvar:
        for exp in listvar[var]:
            if type(exp) != list and isinstance(exp.value, LenCons):
                if exp.value.op == "=" and exp.value.number == 0:
                    list_empty.append(var)

    if len(list_empty) == 0:
        return listvar

    #print list_empty

    for var in listvar:
        t = len(listvar[var])
        for i in range(0, t):
            if type(listvar[var][i]) != list:
                if isinstance(listvar[var][i], FormSingle) and isinstance(listvar[var][i].value, ConcatCons):
                    if check_has_empty_string(listvar[var][i].value, list_empty):
                        listvar[var][i].value = replace_empty_string(listvar[var][i].value, list_empty)
                elif isinstance(listvar[var][i], FormNeg) and isinstance(listvar[var][i].value.value, ContainCons):
                    if check_has_empty_string(listvar[var][i].value.value, list_empty):
                        listvar[var][i].value.value = replace_empty_string(listvar[var][i].value.value, list_empty)
    return listvar
