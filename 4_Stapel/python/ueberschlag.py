import random

from functools import reduce

def subsets(n):
    if n == 1:
        return [[1], [0]]
    else:
        return [x + [0] for x in subsets(n - 1)] + [x + [1] for x in subsets(n - 1)]


def xor(a, b):
    return [x ^ y for x, y in zip(a, b)]


def p(n,k):
    s = subsets(n)
    if k > len(s):
        return None
    total = 50000
    hits = 0
    for i in range(total):
        word = reduce(xor, random.sample(s, k=k), [0]*len(s[0]))
        if sum(word) == 0:
            hits += 1
    print(hits/total)
    return hits/total

import numpy as np
from matplotlib import pyplot as plt

if __name__ == "__main__":
    x = list(range(1, 8))
    for k in [1,2,3,4,5,7,8, 10, 50]:
        plt.plot(x, [p(a,k) for a in x], label=str(k))
    plt.legend(loc="upper right")
    plt.show()
