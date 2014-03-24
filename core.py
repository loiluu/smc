__author__ = 'secnus'


class Constraint:
    def __repr__(self):
        return str(self)

class RegCons(Constraint):
    def __init__(self, _var, _pattern):
        self.var = _var
        self.pattern = _pattern

    def __str__(self):
        return self.var + ' in ' + self.pattern

class LenCons(Constraint):
    def __init__(self, _var, _op, _num):
        self.var = _var
        self.op = _op
        self.number = _num

    def __str__(self):
        return "length(" + self.var + ')' + self.op + str(self.number)


class ConcatCons(Constraint):
    def __init__(self, _var, _var1, _var2):
        self.var = _var
        self.var_1 = _var1
        self.var_2 = _var2

    def __str__(self):
        return self.var + '=' + self.var_1 + '.' + self.var_2

class EqualCons(Constraint):
    def __init__(self, _var, _var1):
        self.var = _var
        self.var_1 = _var1

    def __str__(self):
        return self.var + '=' + self.var_1


class EqualCons2(Constraint):
    def __init__(self, _var, _pattern):
        self.var = _var
        self.pattern = _pattern

    def __str__(self):
        return self.var + '=' + self.pattern

class ContainCons(Constraint):
    def __init__(self, _var, _pattern):
        self.var = _var
        self.pattern = _pattern

    def __str__(self):
        return "contains(" + self.var + ',' + self.pattern+")"

class StrstrCons(Constraint):
    def __init__(self, _var, _pattern, _num):
        self.var = _var
        self.pattern = _pattern
        self.number = _num

    def __str__(self):
        return "strstr(" + self.var + ',' + self.pattern+") = " + str(self.number)

class allString(Constraint):
    def __init__(self, _var):
        self.var = _var
    def __str__(self):
        return self.var + "is arbitrary string"

class Formula:
    def __repr__(self):
        return str(self)

class FormSingle(Formula):
    def __init__(self, t):
        if isinstance(t, Constraint):
            self.value = t
        else:
            pass
    def __str__(self):
        return str(self.value)

class FormNeg(Formula):
    def __init__(self, t):
        if isinstance(t, Formula):
            self.value = t
        else:
            pass

    def __str__(self):
        return "!" + str(self.value)

class FormAnd(Formula):
    def __init__(self, l, r):
        if isinstance(l, Formula) and isinstance(r, Formula):
            self.left = l
            self.right = r
        else:
            pass

    def __str__(self):
        return str(self.left) + " and " + str(self.right)

class FormOr(Formula):
    def __init__(self, l, r):
        if isinstance(l, Formula) and isinstance(r, Formula):
            self.left = l
            self.right = r
        else:
            pass

    def __str__(self):
        return "(" + str(self.left) + " or " + str(self.right) + ")"