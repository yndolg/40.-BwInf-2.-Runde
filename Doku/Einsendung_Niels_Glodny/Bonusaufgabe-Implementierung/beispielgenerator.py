import os.path
import random

if __name__ == "__main__":
    n = int(input("Wie viele Karten sollen es insgesamt sein? "))
    k = int(input("Wie viele sind davon Öffnungskarten? "))
    m = int(input("Wie viele Bits hat jede Karte? "))

    path = input("In welchen Ordner soll das Beispiel gespeichert werden? [leerlassen für './Eingabe']")
    if len(path) == 0:
        path = "Eingabe"

    # Automatisch die Dateinamen finden
    file = 0
    while os.path.exists(f"{path}/stapelc{file}.txt"):
        file += 1

    bsp_path = os.path.abspath(f"{path}/stapelc{file}.txt")
    loe_path = os.path.abspath(f"{path}/stapelc{file}_loesung.txt")

    print(f"Schreibe Beispiel nach '{bsp_path}' und Lösung nach '{loe_path}'")

    karten = []
    for i in range(n - 1):
        karten.append([random.randint(0, 1) for _ in range(m)])

    oeffnungskarten = random.choices(list(range(n - 1)), k=(k + 1))

    sicherheitskarte = [0] * m
    for karte in oeffnungskarten[:-1]:
        sicherheitskarte = [(a + b) % 2 for a, b in zip(karten[karte], sicherheitskarte)]

    karten[oeffnungskarten[-1]] = sicherheitskarte



    with open(bsp_path, "w") as f:
        f.write(f"{n} {k} {m}\n")
        for karte in karten:
            f.write("".join([str(x) for x in karte]) + "\n")

    with open(loe_path, "w") as f:
        f.write(", ".join(map(str, sorted(oeffnungskarten))) + "\n")
