import functools
import math

f = open("Eingabe/stapel2.txt")

n, k, m = [int(x) for x in f.readline().split()]
cards = []
for i in range(n):
    l = f.readline()
    l = filter(lambda x: x.isnumeric(), l)
    cards.append([int(v) for v in l])


selected_bits = []

def calculate_entropy(cards, selected_bits):
    groups = dict()
    for card in cards:
        group = tuple([card[bit] for bit in selected_bits])
        if group not in groups.keys():
            groups[group] = 0
        groups[group] += 1/len(cards)
    
    return - sum([v * math.log2(v) for v in groups.values()])

def is_unique(cards, selected_bits):
    groups = set()
    for card in cards:
        groups.add(tuple([card[bit] for bit in selected_bits]))
    return len(groups) == len(cards)

def select_next_bit(cards, selected_bits):
    max_v = 0
    max_bit = -1
    for i in range(len(cards[0])):
        if i in selected_bits:
            continue # Bit ist schon selected
        v = calculate_entropy(cards, selected_bits=(set([i]) | selected_bits))
        if v > max_v:
            max_v = v
            max_bit = i
    return max_bit

selected = set()
while not is_unique(cards, selected):
    bit = select_next_bit(cards, selected)
    selected.add(bit)
    print(f"Bits: {selected}")

selected = sorted(list(selected))

print(selected)
old_cards = cards
new_cards = []
for card in cards:
    new_cards.append([card[i] for i in selected])

@functools.lru_cache(maxsize=10000, )
def subset_sum(pos, used, value):
    if sum(value) == 0 and used == k:
        return set([tuple()])
    if used == k: # Wenn man keinen mehr verwenden darf, wird nichts mehr drauß
        return set()
    if pos == 0:
        return set()
    v1 = subset_sum(pos-1, used,     value)
    v2 = subset_sum(pos-1, used + 1, tuple([(i + j) % 2 for (i,j) in zip(value, new_cards[pos])]))    
    v2 = set(map(lambda x: x + (pos,), v2))
    return v1 | v2

r = subset_sum(len(new_cards)-1, 0, tuple([0]*len(new_cards[0])))
print(len(r))