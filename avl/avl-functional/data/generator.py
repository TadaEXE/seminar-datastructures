
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

def generateBenchmarkSimple(n):
    items = np.array(range(n))
    random.shuffle(items)
    converted = "i" + items.astype("str")
    commands = []
    for t in range (n // 10000):
        block = converted[t*10000 : (t + 1) * 10000]
        commands += [block]
    random.shuffle(items)
    converted = "d" + items.astype("str")
    for t in range (n // 10000):
        block = converted[t*10000 : (t + 1) * 10000]
        commands += [block]
    return commands

def print_data(commands, path):
    f = open(path, "w+")
    for block in commands:
        block_sequence = "".join(block)
        f.write(block_sequence + "\n")
    f.close()

print_data(generateBenchmarkSimple(10000), "data/benchmark_10000.txt")