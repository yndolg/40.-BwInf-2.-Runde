from functools import reduce
import math
import random
from matplotlib import pyplot as plt
import numpy as np
import seaborn as sn

f = open("Eingabe/stapel2.txt")

n, k, m = [int(x) for x in f.readline().split()]

cards = []
for i in range(n):
    l = f.readline()
    l = filter(lambda x: x.isnumeric(), l)
    cards.append([int(v) for v in l])

random.seed(0)

l = 11
total_samples = 0
marks = [0]*len(cards)

coincedence = np.zeros((len(cards), len(cards)))
local_coincidence = np.zeros((len(cards), len(cards)))

while True:
    # sample l unique positions in the cards
    bits = sorted(random.sample(range(len(cards[0])), k = l))
    
    # filter for only the l positions
    filtered_cards = [[card[bit] for bit in bits] for card in cards]

    submarks = dict()
    counts = dict()
    for value in filtered_cards:
        submarks[tuple(value)] = 0
        if not tuple(value) in counts:
            counts[tuple(value)] = 0
        counts[tuple(value)] += 1

    total_samples += 1

    # reapeat often: 
    for i in range(100):
        # sample k times from the filtered cards
        sample = random.sample(range(len(cards)), k = k + 1)
        
        # calculate xored value
        xored = reduce(lambda a, b: [x ^ y for x,y in zip(a,b)], [filtered_cards[card] for card in sample])

        # if it works:
        if(sum(xored) == 0):
            # mark all the included cards 
            for included in sample:
                value = tuple(filtered_cards[included])
                submarks[value] += 1

            # mark in coincedence matrix
            for a in sample:
                for b in sample:
                    if a != b:
                        local_coincidence[a,b] += 1
        
        
    marks_new = [submarks[tuple(card)]/counts[tuple(card)] for card in filtered_cards]
    # plt.bar(range(len(cards)), marks_new)
    # plt.savefig(f"vv{total_samples}local.png")
    # plt.close()

    marks = [a+(b/10000) for a,b in zip(marks, marks_new)]
    if total_samples % 1000 == 0:
        # plt.bar(range(len(cards)), [x for x in marks])
        # plt.xticks(range(len(cards)))
        # plt.savefig(f"vv{total_samples}total.png")
        # plt.close()

        sn.heatmap(local_coincidence, xticklabels = range(0,len(cards)), yticklabels = range(0,len(cards)))
        plt.savefig(f"vv{total_samples}conf.png")
        plt.close()

    
    
    print(" ".join(["{:.2f}".format(x/total_samples*1000) for x in marks]))
