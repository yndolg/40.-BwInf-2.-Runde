import copy
import itertools
import random
from typing import Dict

import numpy as np
import galois

import math

# Source: https://www.youtube.com/watch?v=EqRsel-rXac
def inv_permutation(p):
    s = np.empty_like(p)
    s[p] = np.arange(p.size)
    return s


def read_H(path):
    with open(path) as f:
        k, d, n = [int(x) for x in f.readline().split()]
        M = galois.GF2(np.zeros((k, n), int))
        for i in range(k):
            v = [int(x) for x in f.readline().strip()]
            M[i, :] = v

    H = galois.GF2(M.transpose())

    H = H.row_reduce()
    # remove zero-rows
    rows = []
    for i in range(H.shape[0]):
        if sum(np.array(H[i, :])) > 0:
            rows.append(H[i, :])
    # Bring into form
    H = np.vstack(rows)

    return galois.GF2(H), d + 1


H_orig, t = read_H("../Eingabe/stapel5.txt")

prob = (math.comb(H_orig.shape[1] - H_orig.shape[0], 1)
        * math.comb(H_orig.shape[0], t-1)) \
       / math.comb(H_orig.shape[1], t)

print(f"P(Success) = {prob}, Expected tries: {1/prob}")

tries = 0
running = True
while running:
    # permute the columns
    tries += 1
    H = copy.copy(H_orig)

    permutation = np.random.permutation(H.shape[1])
    H = H[:, permutation]

    H = H.row_reduce()
    # find pivot elements
    m_i, k_i = [], []
    for row in range(H.shape[0]):
        if np.array(H[row, :]).sum() != 0:
            col = list(H[row, :]).index(1)
            m_i.append(col)
            k_i.append(row)

    # select column p that was not in pivots
    for p in list(set(range(H.shape[1])) - set(m_i)):
        # if column p has weight of exactly t - 1 --> match
        s = np.array(H[:, p])
        if s.sum() == t - 1:
            solution = np.zeros(H.shape[1])
            # solution has bit p set and the columns of the pivots in the rows of the syndrome (p)
            solution[p] = 1
            for i in range(H.shape[0]):
                if s[k_i[i]] == 1:
                    solution[m_i[i]] = 1

            # sanity check
            # unpermute the solution
            solution = solution[inv_permutation(permutation)]
            print("Found a solution! ", np.flatnonzero(solution))
            print(f"Number of tries: {tries}")
            running = False
            break
        # else: repeat from beginning, new column permutation
        else:
            pass
            # print("No success, retrying....")
