import re
import sys
f = open("pcre_output.txt", "r")
domain = "X"
gf_dict = {}

def get_gf_for_block(block):
    block = block.strip()

    #if the block is computed before
    if re.match("^\_([a-zA-Z])*\d+$", block):
        return gf_dict[block]
    #if the block has the form (block)X where X in [*, +, ?]
    try:
        inner_key = re.search("\((.*?)\)(\+|\*|\?)", block).group(1)        
        if "*" in block:            
            return "1/(1-(" + gf_dict[inner_key] + "))"
        elif "+" in block:            
            return "1/(1-(" + gf_dict[inner_key] + "))-1"
        elif "?" in block:
            return "1+" + gf_dict[inner_key]
        else:
            raise "Still haven't handled yet"
    except AttributeError:
        pass        
    
    #if the block has the form [\xxx - \yyy]
    if '-' in block:
        tmp = re.findall(r'\d+', block)
        return str(int(tmp[1]) - int(tmp[0]) + 1) + domain
    
    #else the block has the form of concatenation
    
    #for the case ()|A|B
    if len(block) == 0:
        return "1"
    tmp = block.split()        
    #this is the concatenation of several blocks
    if not (tmp[0] in gf_dict):
        return domain + "^" + str(len(tmp))
    else:
        return "(" + ")*(".join(gf_dict[i] for i in tmp) + ")"
        
    raise "Still haven't handled yet"

for line in f:
    new_line = re.sub(";", "", line)
    name = new_line.split()[1]
    compos = new_line.split(":=")[1].split("|")
    tempgf = ""
    for component in compos:
        if len(tempgf):
            tempgf = tempgf + "+" + get_gf_for_block(component)
        else:
            tempgf = get_gf_for_block(component)
    gf_dict[name] = tempgf
    # print line, name, gf_dict[name]   

f.close()

if __name__ == "__main__":
    output = open("gf_regex.txt", "w")
    query_bracket = "_flax" + sys.argv[1]
    output.write(gf_dict[query_bracket])
    output.close()
