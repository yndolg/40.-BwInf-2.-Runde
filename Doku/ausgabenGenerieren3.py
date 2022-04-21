import subprocess

for x in [0,1,2,3,4,5,"c0", "c1", "c2", ]:
    name = f"hexmax{x}.txt"
    print(f"Running '{name}'...")
    for alg in ["greedy", "kurz", None]:
        if x in [5, "c0", "c1", "c2", "c3"] and alg is not None:
            continue
        outname = f"./Ausgaben/hexmax{x}{'_umlegungen_'+ alg if alg is not None else ''}.txt"
        with open(outname, "w") as f:
            f.write(f"$ ./hexmax {'-v ' +alg if alg is not None else ''} ../Eingabe/{name}\n")
            f.close()
        subprocess.run(f"../3_Hexmax/cmake-build-release/hexmax {'-v ' +alg if alg is not None else ''} ../3_Hexmax/Eingabe/{name} >> {outname}", shell=True)