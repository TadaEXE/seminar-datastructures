import re
import os
from matplotlib import pyplot as plt

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# OCaml benchmark runs 4_000_000 repetitions total, 1 word = 8 bytes on 64-bit
REPS = 4_000_000
WORD_SIZE = 8



def read_data(path):
    data = {}
    with open(path) as f:
        for line in f:
            line = line.strip().rstrip(",")
            m = re.match(r'"([^"]+)":\[(.*)\]', line)
            if not m:
                continue
            key = m.group(1)
            pairs = re.findall(r"\(([^)]+)\)", m.group(2))
            data[key] = [(float(p.split(",")[0]), float(p.split(",")[1])) for p in pairs]
    return data


def avg(vals):
    vals = [v for v in vals if v > 0]
    return sum(vals) / len(vals) if vals else 0.0


ocaml = read_data(os.path.join(SCRIPT_DIR, "results_comp.txt"))
cpp   = read_data(os.path.join(SCRIPT_DIR, "../../avl-imperative/results_comp_cpp.txt"))

depths = list(range(15, 35))

# OCaml: minor_words -> bytes per operation
ocaml_i_opt   = [avg([s[1] for s in ocaml[f"i_opt_{n}"]]) * WORD_SIZE / REPS for n in depths]
ocaml_i_worst = [avg([s[1] for s in ocaml[f"i_worst_{n}"]]) * WORD_SIZE / REPS for n in depths]
ocaml_d_opt   = [avg([s[1] for s in ocaml[f"d_opt{n}"]]) * WORD_SIZE / REPS for n in depths]
ocaml_d_worst = [avg([s[1] for s in ocaml[f"d_worst{n}"]]) * WORD_SIZE / REPS for n in depths]

# C++: actual bytes per insert measured via operator new override
cpp_i_opt   = [avg([s[1] for s in cpp[f"i_opt_{n}"]]) for n in depths]
cpp_i_worst = [avg([s[1] for s in cpp[f"i_worst_{n}"]]) for n in depths]

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
fig.suptitle("AVL memory allocation per operation")

ax1.set_title("Insert")
ax1.plot(depths, ocaml_i_opt,   "r--", label="OCaml best")
ax1.plot(depths, ocaml_i_worst, "b--", label="OCaml worst")
ax1.plot(depths, cpp_i_opt,     "r-",  label="C++ best")
ax1.plot(depths, cpp_i_worst,   "b-",  label="C++ worst")
ax1.set_xlabel("tree depth")
ax1.set_ylabel("allocated bytes per operation (log scale)")
ax1.set_yscale("log")
ax1.legend()

ax2.set_title("Delete")
ax2.plot(depths, ocaml_d_opt,   "r--", label="OCaml best")
ax2.plot(depths, ocaml_d_worst, "b--", label="OCaml worst")
# C++ delete: 0 bytes allocated (in-place), not shown on log scale
ax2.set_xlabel("tree depth")
ax2.set_ylabel("allocated bytes per operation (log scale)")
ax2.set_yscale("log")
ax2.legend()

plt.tight_layout()
plt.savefig(os.path.join(SCRIPT_DIR, "memory_comparison.png"), dpi=160)
plt.show()
