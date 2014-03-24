__author__ = 'secnus'

def get_all_sub_strings_insensitive(t):
    result = []
    for i in range(0, len(t)):
        for j in range(i, len(t) + 1):
            tmp_str = t[i:j]
            tmp_list = get_all_case_strings(tmp_str)
            for i_tmp in tmp_list:
                if not (i_tmp) in result:
                    result.append(i_tmp)
    return result

def get_all_sub_strings_original(t):
    result = []
    for i in range(0, len(t)):
        for j in range(i, len(t) + 1):
            tmp_str = t[i:j]
            if not (tmp_str) in result:
                result.append(tmp_str)
    return result

def get_case_regex_not_null(t):
    result = ""
    for i in t:
        if i.lower() == i.upper():
            result += i
        else:
            result += "(" + i.lower() + '|' + i.upper() + ")"
    return result

def get_regex(t):
    result = ""
    for i in t:
        result += "(" + i + "|$)"
    return result

def get_case_number(t):
    result = 1
    for i in t:
        if i.lower() != i.upper():
            result *= 2
    return result


def get_all_case_strings(t):
    result = [[], []]
    index = 0
    for i in t:
        if len(result[index]):
            for item in result[index]:
                if i.lower() == i.upper():
                    result[1-index].append(item + i)
                else:
                    result[1-index].append(item + i.lower())
                    result[1-index].append(item + i.upper())
        else:
            if i.lower() == i.upper():
                result[1-index].append(i.lower())
            else:
                result[1-index].append(i.lower())
                result[1-index].append(i.upper())
        result[index][:] = []
        index = 1 - index
    return result[index]
