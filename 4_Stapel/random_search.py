from audioop import reverse
import copy
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

random.seed(1)

l = 7
scores = dict()

def evaluate(solution):
    works, total = scores.get(tuple(solution), [0,0])
    for i in range(50):
        # sample l unique positions in the cards
        bits = sorted(random.sample(range(len(cards[0])), k = l))
        # filter for only the l positions
        filtered_cards = [[card[bit] for bit in bits] for card in cards]

        # sample k + 1 times from the filtered cards
        sample = solution #random.sample(range(len(cards)), k = k + 1)

        # calculate xored value
        xored = reduce(lambda a, b: [x ^ y for x,y in zip(a,b)], [filtered_cards[card] for card in sample])

         # if it works:
        if(sum(xored) == 0):
            works += 1
        total += 1
    scores[tuple(solution)] = (works, total)
    return works/total

def get_neighboring_solution(solution, length):
    return [min(max(x + random.randint(-1, 1), 0), length-1) for x in solution]

    
solution = [0,1,2,3,11]
score = 0
next_solution = solution
while True: 
    while True:
        next_solutions = [get_neighboring_solution(get_neighboring_solution(solution, n),n) for _ in range(100)] + [solution]
        with_scores = sorted([(evaluate(s), s) for s in next_solutions], key=lambda x: x[0], reverse=True)
        new_score = with_scores[0][0]
        solution = with_scores[0][1]
        print(f"{solution}, score: {new_score}")

        # sn.heatmap(local_coincidence, xticklabels = range(0,len(cards)), yticklabels = range(0,len(cards)))
        # plt.savefig(f"vv{total_samples}conf.png")
        # plt.close()

    
    
    # print(" ".join(["{:.2f}".format(x/total_samples*1000) for x in marks]))
