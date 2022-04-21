import subprocess
import tqdm

for x in [0,1,2,3,4,5]:
    name = f"stapel{x}.txt"
    for alg in ["bruteforce", "isd"]:
        
        if alg != "isd" and x in ["c0"]:
            continue
        print(f"Führe {name} mit {alg} aus...")
        outname = f"./Ausgaben/stapel{x}_{alg}.txt"
        with open(outname, "w") as f:
            f.write(f"$ ./stapel {alg} ../Eingabe/{name}\n")
            f.close()
        subprocess.run(f"../4_Stapel/cmake-build-release/stapel {alg} ../4_Stapel/Eingabe/{name} >> {outname}", shell=True)