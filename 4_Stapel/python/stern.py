import itertools
import random
from typing import Dict

import numpy as np
import galois

p = 2
l = 8

w = 10


def read_H(path):
    n = 0
    k = 0
    m = 0
    with open(path) as f:
        k, d, n = [int(x) for x in f.readline().split()]
        M = galois.GF2(np.zeros((k, n), int))
        for i in range(k):
            v = [int(x) for x in f.readline().strip()]
            M[i, :] = v

    H = galois.GF2(M.transpose())
    return n, k, H


def s1_guassian_elimination(H):
    # perform gaussian elimination
    H = H.row_reduce()

    # remove zero-rows
    rows = []
    for i in range(H.shape[0]):
        if sum(np.array(H[i, :])) > 0:
            rows.append(H[i, :])
    # Bring into form
    H = np.vstack(rows)

    # find pivot elements
    m_i, k_i = [], []
    for row in range(H.shape[0]):
        if np.array(H[row, :]).sum() != 0:
            col = list(H[row, :]).index(1)
            m_i.append(col)
            k_i.append(row)
    return H, m_i, k_i


def s2_split(H, n, k, pivots):
    X, Y = set(), set()
    for column in set(range(H.shape[1])) - set(pivots):
        if random.random() < 0.5:
            X.add(column)
        else:
            Y.add(column)
    return X, Y


def s4_calc_pi(H, X, Y):
    pi = dict()  # sorted tuple --> list
    for set in [X, Y]:
        for A in itertools.combinations(set, p):
            pi[tuple(sorted(A))] = H[:, A].sum(axis=1)
    return pi


def s5_solve(H, X, Y, pi: Dict, m_i, k_i):
    for A in itertools.combinations(X, p):
        for B in itertools.combinations(Y, p):
            if np.array_equal(pi[tuple(sorted(A))], pi[tuple(sorted(B))]):
                union = list(set(A) | set(B))

                V = H[:, union].sum(axis=1)
                print(list(V).count(1))
                if list(V).count(1) == w - 2 * p:
                    x = [1 if (m in union) or
                              any([m == m_i[i] and V[k_i[i]] for i in range(H.shape[1])])
                         else 0
                         for m in range(H.shape[1])]
                    return x
    return False


x = False
while True:
    print("Attempt started.")
    n, k, H = read_H("../Eingabe/stapel1.txt")
    H = galois.GF2(np.random.permutation(H.T).T)
    H, m_i, k_i = s1_guassian_elimination(H, )
    X, Y = s2_split(H, n, k, m_i)
    pi = s4_calc_pi(H, X, Y)
    x = s5_solve(H, X, Y, pi, m_i, k_i)
    if x:
        break
print(x)
