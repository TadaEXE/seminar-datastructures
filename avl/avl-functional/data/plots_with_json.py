import json
import re
import os
from matplotlib import pyplot as plt

pref = "../../avl-imperative/"
paths = ["mixed_cpp_recursive.txt", "mixed_cpp_iterative.txt", "mixed_cpp_vector.txt", "mixed_cpp_compact.txt"]
insKey = "cpp_"
delKey = "cpp_"
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
def avg(values, offset):
    vals = [values[i][0] for i in range(offset, len(values), 2) if values[i][0] > 0]
    return sum(vals) / len(vals) if vals else 0

# depths = range(50000, 500001, 50000)
# depths = range(15,35)
depths = range(15, 26)

# C++ already in seconds per operation -> convert to nanoseconds
rec_ins_avg   = [avg(data[0][f"{insKey}{n}"], 0) * 1e9 for n in depths]
rec_del_avg   = [avg(data[0][f"{delKey}{n}"], 1) * 1e9 for n in depths]
it_ins_avg   = [avg(data[1][f"{insKey}{n}"], 0) * 1e9 for n in depths]
it_del_avg   = [avg(data[1][f"{delKey}{n}"], 1) * 1e9 for n in depths]
vec_ins_avg   = [avg(data[2][f"{insKey}{n}"], 0) * 1e9 for n in depths]
vec_del_avg   = [avg(data[2][f"{delKey}{n}"], 1) * 1e9 for n in depths]
comp_ins_avg   = [avg(data[3][f"{insKey}{n}"], 0) * 1e9 for n in depths]
comp_del_avg   = [avg(data[3][f"{delKey}{n}"], 1) * 1e9 for n in depths]

# --- plot ---
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
fig.suptitle("OCaml functional vs C++ imperative AVL tree")

ax1.set_title("Insert: best vs worst case")
ax1.plot(depths, rec_ins_avg,   "r--", label="Recursive")
ax1.plot(depths, it_ins_avg, "b--", label="Iterative")
ax1.plot(depths, vec_ins_avg,   "g--", label="Vector")
ax1.plot(depths, comp_ins_avg, "y--", label="Compact")
ax1.set_xlabel("tree depth (n)")
ax1.set_ylabel("time per operation (depths)")
ax1.legend()

ax2.set_title("Delete: best vs worst case")
ax2.plot(depths, rec_del_avg,   "r--", label="Recursive")
ax2.plot(depths, it_del_avg, "b--", label="Iterative")
ax2.plot(depths, vec_del_avg,   "g--", label="Vector")
ax2.plot(depths, comp_del_avg, "y--", label="Compact")
ax2.set_xlabel("tree depth (n)")
ax2.set_ylabel("time per operation (depths)")
ax2.legend()

plt.tight_layout()
plt.show()
