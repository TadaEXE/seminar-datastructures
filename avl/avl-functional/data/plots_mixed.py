import json
import re
import os
from matplotlib import pyplot as plt

def getNodesUnbalanced(n):
    nodes = [0, 1]
    for i in range(1, n):
        nodes += [nodes[i] + nodes[i - 1] + 1]
    return nodes

pref = "./"
paths = ["mixed_cpp_recursive.txt", "mixed_cpp_iterative.txt", "mixed_cpp_vector.txt", "mixed_cpp_compact.txt"]
paths = [
    "../avl-imperative/amortized_cpp_recursive.txt",
    "../avl-imperative/amortized_cpp_imperative.txt",
    "../avl-imperative/amortized_cpp_vector.txt",
    "../avl-imperative/amortized_cpp_compact.txt",
]
paths = ["../avl-imperative/mixed_cpp_recursive.txt", 
         "../avl-imperative/mixed_cpp_iterative.txt",
         "../avl-imperative/mixed_cpp_vector.txt",
         "../avl-imperative/mixed_cpp_compact.txt",
         "./data/results_ordered_1.txt",
         "./data/results_unbalanced.txt"]
worstKey = "worst_"
bestKey = "best_"
data = []

for path in paths:
    f = open(pref + path)
    content = f.read()
    content = content.replace("(", "[").replace(")", "]")
    content = "{" + content[:-2] + "}"
    f.close()
    #print(content)
    data += [json.loads(content)]

print(type(data[0]))
# average 5 reps, skip zeros (OCaml sometimes has 0.0 as failed measurement)
def avg(values, step, offset):
    vals = [values[i][1] for i in range(offset, len(values), step) if values[i][0] > 0]
    return sum(vals) / len(vals) if vals else 0

# depths = range(50000, 500001, 50000)
# depths = range(15,35)
depths1 = range(15, 26)
depths2 = range(21, 36)

key = worstKey
depths = depths2

fact = 0.001

insRec = [avg(data[0][f"{key}{n}"], 2, 0) * fact for n in depths]
insIt = [avg(data[1][f"{key}{n}"], 2, 0) * fact for n in depths]
insVec = [avg(data[2][f"{key}{n}"], 2, 0) * fact for n in depths]
insComp = [avg(data[3][f"{key}{n}"], 2, 0) * fact for n in depths]
delRec = [avg(data[0][f"{key}{n}"], 2, 1) * fact for n in depths]
delIt = [avg(data[1][f"{key}{n}"], 2, 1) * fact for n in depths]
delVec = [avg(data[2][f"{key}{n}"], 2, 1) * fact for n in depths]
delComp = [avg(data[3][f"{key}{n}"], 2, 1) * fact for n in depths]

insFun1 = [avg(data[4][f"Balanced_{n}"], 2, 0) / 500000 * fact for n in depths1]
insFun2 = [avg(data[5][f"Unbalanced_{n}"], 2, 0) / 500000 * fact for n in depths2]
delFun1 = [avg(data[4][f"Balanced_{n}"], 2, 1) / 500000 * fact for n in depths1]
delFun2 = [avg(data[5][f"Unbalanced_{n}"], 2, 1) / 500000 * fact for n in depths2]


nodes1 = [pow(2, d) - 1 for d in depths1]
nodes2 = getNodesUnbalanced(35)[21:]
nodes = nodes2

# --- plot ---
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
fig.suptitle("AVL: Memory of functional and imperative implementations")

ax1.set_title("Insert: Average memory allocated by variant")
ax1.plot(nodes, insFun2,   "m--", label="Functional")
ax1.plot(nodes, insRec,   "r--", label="Recursive")
ax1.plot(nodes, insIt, "b--", label="Iterative")
ax1.plot(nodes, insVec,   "g--", label="Vector")
ax1.plot(nodes, insComp, "y--", label="Compact")
ax1.set_xlabel("nodes")
ax1.set_ylabel("memory (KB)")
ax1.legend()

ax2.set_title("Delete: Average memory allocated by variant")
ax2.plot(nodes, delFun2,   "m--", label="Functional")
ax2.plot(nodes, delRec,   "r--", label="Recursive")
ax2.plot(nodes, delIt, "b--", label="Iterative")
ax2.plot(nodes, delVec,   "g--", label="Vector")
ax2.plot(nodes, delComp, "y--", label="Compact")
ax2.set_xlabel("#nodes")
ax2.set_ylabel("memory (KB)")
ax2.legend()

plt.tight_layout()
plt.show()
