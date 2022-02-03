

from functools import reduce


f = open("Eingabe/stapel5.txt")

n, k, m = [int(x) for x in f.readline().split()]
cards = []
for i in range(n):
    l = f.readline()
    l = filter(lambda x: x.isnumeric(), l)
    cards.append([int(v) for v in l])

solution = [70, 77, 163, 167, 185]

# Stapel 0: [2, 3, 5, 9, 11]
# Stapel 1: [1, 2, 4, 6, 7, 9, 11, 14, 15]
# Stapel 2: [9, 20, 26, 53, 57, 71, 76, 78, 89, 95, 99]
# Stapel 3: [23, 26, 34, 43, 71, 98, 110, 120, 127, 133, 144]
# Stapel 4: [0, 23, 76, 83, 91, 95, 116, 146, 154, 159, 160]
# Stapel 5: [70, 77, 163, 167, 185]
xored = reduce(lambda a, b: [x ^ y for x,y in zip(a,b)], [cards[card] for card in solution])
print(xored)