import functools
import math

f = open("Eingabe/stapel0.txt")


# n, k, m = [int(x) for x in f.readline().split()]
# cards = []
# for i in range(n):
#     l = f.readline()
#     l = filter(lambda x: x.isnumeric(), l)
#     cards.append([int(v) for v in l])
#

def subsets(n):
    if n == 1:
        return [[1], [0]]
    else:
        return [x + [0] for x in subsets(n - 1)] + [x + [1] for x in subsets(n - 1)]


cards = subsets(4)


@functools.lru_cache(maxsize=100000, )
def subset_sum(pos, used, value):
    if sum(value) == 0 and used == 0:
        return {tuple([0]*len(cards))}
    if used == 0:  # Wenn man keinen mehr verwenden darf, wird nichts mehr draus
        return set()
    if pos == -1:
        return set()

    all = set()
    v1 = subset_sum(pos - 1, used, value)
    all = all.union(v1)

    new_val = tuple([(i + j) % 2 for (i, j) in zip(value, cards[pos])])
    v2 = subset_sum(pos - 1, used - 1, new_val)
    v3 = subset_sum(pos, used - 1, new_val)

    for x in v2 | v3:
        l = list(x)
        l[pos] += 1
        all.add(tuple(l))

    return all


r = subset_sum(len(cards) - 1, 10, tuple([0] * len(cards[0])))

sum = 0
for word in r:
    mult = 1
    for pos in word:
        mult *= math.comb(math.ceil(181/len(cards)), pos)
    sum += mult

print(len(r), "/", 2**len(cards))
print(sum / math.comb(181, 10))
