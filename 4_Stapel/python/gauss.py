import copy
from statistics import mode

f = open("../Eingabe/stapel4.txt")

n, K, m = [int(x) for x in f.readline().split()]

cards = []
for i in range(n):
    l = f.readline()
    l = filter(lambda x: x.isnumeric(), l)
    cards.append([int(v) for v in l])

from ortools.sat.pywrapsat import CpSatHelper_ValidateModel
from ortools.sat.python import cp_model

count = 0


def gauss(matrix):
    # To echelon form
    h = 0
    k = 0
    while h < matrix.shape[0] and k < matrix.shape[1]:
        # Find Pivot
        i_max = h
        while i_max < matrix.shape[0] and matrix[i_max, k] == 0:
            i_max += 1
        if i_max == matrix.shape[0]:
            k = k + 1
        else:
            # swap rows h and i_max
            matrix[h, :], matrix[i_max, :] = matrix[i_max, :], matrix[h, :]
            # for all rows below pivot
            for i in range(h + 1, matrix.shape[0]):
                f = matrix[i, k] / matrix[h, k]
                # dividing, but not nesseccary in F2
                # matrix[i, k] = 0
                for j in range(k, matrix.shape[1]):
                    matrix[i, j] = (matrix[i, j] + matrix[h, j] * f) % 2  # XOR
            h = h + 1
            k = k + 1

    # Re-Substitution
    n_vars = matrix.shape[1] - 1
    solution = [0] * n_vars
    row_of_var = dict()
    for i in range(matrix.shape[0]):
        ones = np.where(matrix[i])[0]
        if len(ones) > 0:
            first_one = ones[0]
            row_of_var[first_one] = i

    model = cp_model.CpModel()

    print(f"Found {n_vars - len(row_of_var)} undetermined variables.")
    for var in range(n_vars - 1, -1, -1):
        solution[var] = model.NewBoolVar(f"x{var}")
        if not var in row_of_var.keys():
            pass
        else:
            row = row_of_var[var]
            model.AddBoolXOr([a for a, b in zip(solution, matrix[row]) if b == 1] + [1])

    model.Add(sum(solution) == K + 1)
    solver = cp_model.CpSolver()
    status = solver.Solve(model)

    print(model.Validate())
    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        print("Found OR-Tools solution:", [solver.Value(x) for x in solution])
        return [np.array([solver.Value(x) for x in solution]).nonzero()[0]]
    else:
        print('No OR-Tools solution found.', solver.StatusName(status))

    solution = [0] * n_vars

    def resub(var, solution):
        global count

        if sum(solution) > K + 1:
            return []

        count += 1
        if count % 50000 == 0:
            print(count, solution)

        if var < 0:
            return [copy.copy(solution)]

        if not var in row_of_var.keys():
            solution[var] = 0
            u = []
            u += resub(var - 1, solution)
            solution[var] = 1
            u += resub(var - 1, solution)
            solution[var] = 0
            return u
        else:
            row = row_of_var[var]
            ones_in_row = matrix[row].nonzero()[0][1:]  # Die erste 1 zählt nicht, das sollte die aktuelle Variable sein
            v = 0
            for solved_var in ones_in_row:
                v = (v + solution[solved_var]) % 2  # XOR
            solution[var] = v
            result = resub(var - 1, solution)
            solution[var] = 0
            return result

    solution = resub(n_vars - 1, solution)

    x = []
    for row in solution:
        x.append(np.array(row).nonzero()[0])

    return x


import numpy as np

# matrix = np.array([[1,1,0,1],[1,0,1,0],[1,1,1,1]])
matrix = np.transpose(np.array(cards))
matrix = np.hstack((matrix, np.zeros((matrix.shape[0], 1))))


def test(cards, solution):
    selected_cards = np.asarray(cards).take(solution, axis=0)
    s = np.mod(np.sum(selected_cards, axis=0), 2)
    if sum(s) == 0:
        return True
    return False


solutions = gauss(matrix)

for s in solutions:
    print("Solution: ", s)
    print("Check   : ", test(cards, s))

# for card in range(len(cards)):
#     for i in range(len(cards[card])):
#         matrix[i, card] = cards[card][i]
np.savetxt("out.csv", matrix, fmt="%d")
