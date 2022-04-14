import operator
import copy
import math
import random

def div(a,b):
    if a % b != 0:
        return math.nan
    else:
        return a // b

OPS = {"*": operator.mul, "/": div, "+": operator.add, "-":operator.sub}

def evaluate(term):
    while "*" in term or "/" in term:
        index = min([term.index(a) if a in term else len(term)+1 for a in "*/"])
        try: 
            term[index-1:index+2] = [OPS[term[index]](term[index-1], term[index+1])]
        except:
            return math.nan

    while "+" in term or "-" in term:
        index = min([term.index(a) if a in term else len(term)+1 for a in "+-"])
        
        term[index-1:index+2] = [OPS[term[index]](term[index-1], term[index+1])]
    return term[0]

solutions = dict()
random.seed(0)
numbers = [6,6,7,8,6,8]#random.randint(2,9) for _ in range(20)]

def recurse(val, i):
    if math.isnan(evaluate(copy.copy(val))):
        return
    if i >= 6:
        value = evaluate(copy.copy(val))
        if math.isnan(value) or value < 0:
            return 
        if value not in solutions:
            solutions[value] = "".join([str(x) for x in val])
        else:
            solutions[value] = ""

        return

    for op in "+-*/":
        recurse(val + [op, numbers[i]],i+1)

recurse([numbers[0]],1)

for k,v in sorted(solutions.items(), key=lambda x: x[0]):
    if len(v) > 0:
        print(f"{v} = {k}")