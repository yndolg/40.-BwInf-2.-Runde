import networkx as nx
import matplotlib.pyplot as plt


G = nx.Graph()

with open("Daten/muellabfuhr8.txt") as f:
    n, m = [int(x) for x in next(f).split()]
    for i in range(0, n):
        G.add_node(i)

    for line in f:
        a, b, c = [int(x) for x in line.split()]
        G.add_edge(a, b, weight=c)

#euler = nx.eulerize(G)
#circuit = nx.eulerian_circuit(euler)
#for edge in circuit:
#    print(edge)
options = {
    'node_size': 20,
    'width': 1,
}

for node in G.edges.data():
    node[2]["w"] = 1/node[2]["weight"]

pos = nx.spring_layout(G, iterations=100, seed=1722, weight="w")

nx.draw(G,pos=pos,**options)

plt.show()
