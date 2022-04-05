import random

N = 10000


if __name__ == "__main__":
    word = ""
    for i in range(N):
        word += random.choice("0123456789ABCDEF")

    with open("Eingabe/hexmaxc3.txt", 'w') as f:
        f.write(word + "\n")
        f.write(str(random.randint(0.5 * N, 2 * N)) + "\n")