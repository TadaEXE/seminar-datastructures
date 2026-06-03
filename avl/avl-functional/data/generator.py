
import random 
import numpy as np

def generateTest(n):
    added = set({})
    commands = []
    trueI = 0
    trueD = 0
    for t in range(4 * n):
        command = 0
        if len(added) > 0:
            command = random.randint(0,2)
        else: 
             command = random.randint(0,1)
        if command == 0:
            x = random.randint(-n // 2, n // 2)
            if x not in added:
                trueI += 1
                added.add(x)
            commands += [f'i{x}']
        elif command == 1:
            x = random.randint(-n // 2, n // 2)
            if x in added:
                trueD += 1
                added.remove(x)
            commands += [f'd{x}']
        else:
            x = random.choice(list(added))
            commands += [f'f{x}']
    return [commands]

def benchmarksInsert(n, step, added):
    block = []
    for t in range(10000):
        x = random.randint(0, (n + 1) * step)
        block += [f"i{x}"]
        added.add(x)
    return block

def benchmarksDelete(n, step, added):
    block = []
    for t in range(10000):
        x = random.randint(step, n * step + len(added) - 1)
        if x <= n * step:
            block += [f"d{x}"]
        else:
            elem = list(added)[x - n * step]
            block += [f"d{elem}"]
            added.remove(elem)
    return block


def benchmarksForTree(n, step, order):
    commands = []
    added = set({})
    for i in range (15):
        if order:
            commands += [benchmarksInsert(n, step, added)]
            commands += [benchmarksDelete(n, step, added)]
        else:
            commands += [benchmarksInsert(n, step, added)]
            commands += [benchmarksDelete(n, step, added)]
    return commands

def print_data(commands, path):
    f = open(path, "w+")
    for block in commands:
        block_sequence = "".join(block)
        f.write(block_sequence + "\n")
    f.close()

#print_data(benchmarksForTree(28656, 64, True), "data/benchmark_15.txt")
#print_data(benchmarksForTree(14930351, 64, True), "data/benchmark_24.txt")
#print_data(benchmarksForTree(832039, 64, True), "data/benchmark_20.txt")
#print_data(benchmarksForTree(24157816, 64, True), "data/benchmark_insert.txt")

for t in range(15, 26):
    print(f"({t}, \"data/range_{pow(2, t) - 1}.txt\");")
    print_data(benchmarksForTree(pow(2, t) - 1, 64, True), f"data/range_{pow(2, t) - 1}_long.txt")