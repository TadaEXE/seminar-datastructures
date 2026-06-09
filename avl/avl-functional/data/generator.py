
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
        x = random.randint(0, n + len(added) - 1)
        if x <= n:
            block += [f"d{x * step}"]
        else:
            elem = list(added)[x - n]
            block += [f"d{elem}"]
            added.remove(elem)
    return block


def benchmarksForTree(n, step, order):
    commands = []
    added = set({})
    for i in range (5):
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

def deleteRandom(n, step):
    commands = []
    for i in range(10000):
        x = random.randint(0, n)
        commands += [f"d{x * step}"]  
    return [commands]  

def insertRandom(n, step):
    commands = []
    for i in range(10000):
        x = random.randint(0, n * step)
        commands += [f"i{x}"]  
    return [commands]  

def deleteOrdered(init, step):
    commands = []
    for i in range(10000):
        commands += [f"d{init + i * step}"]
    return [commands]

def insertOrdered(init, step):
    commands = []
    for i in range(10000):
        commands += [f"i{init + (i + 1) * step}"]
    return [commands]

#print_data(benchmarksForTree(28656, 64, True), "data/benchmark_15.txt")
#print_data(benchmarksForTree(14930351, 64, True), "data/benchmark_24.txt")
#print_data(benchmarksForTree(832039, 64, True), "data/benchmark_20.txt")
#print_data(benchmarksForTree(24157816, 64, True), "data/benchmark_insert.txt")

"""
for t in range(15, 26):
    print(f"({t}, \"data/range_{pow(2, t) - 1}.txt\");")
    print_data(benchmarksForTree(pow(2, t) - 1, 64, True), f"data/range_{pow(2, t) - 1}.txt")

"""
print_data(insertOrdered(0, -64), "./data/insert_min.txt")
print_data(deleteOrdered(0, 64), "./data/delete_min.txt")

print_data(insertOrdered(225572928, 64), f"./data/insert_max{31}.txt")
print_data(deleteOrdered(225572928, -64), f"./data/delete_max{31}.txt")

print_data(insertOrdered(364984704, 64), f"./data/insert_max{32}.txt")
print_data(deleteOrdered(364984704, -64), f"./data/delete_max{32}.txt")

print_data(insertOrdered(590557696, 64), f"./data/insert_max{33}.txt")
print_data(deleteOrdered(590557696, -64), f"./data/delete_max{33}.txt")

print_data(insertOrdered(955542464, 64), f"./data/insert_max{34}.txt")
print_data(deleteOrdered(955542464, -64), f"./data/delete_max{34}.txt")

print_data(insertOrdered(1546100224, 64), f"./data/insert_max{35}.txt")
print_data(deleteOrdered(1546100224, -64), f"./data/delete_max{35}.txt")


print_data(insertRandom(3524577, 64), f"./data/insert_rand{31}.txt")
print_data(deleteRandom(3524577, 64), f"./data/delete_rand{31}.txt")

print_data(insertRandom(5702886, 64), f"./data/insert_rand{32}.txt")
print_data(deleteRandom(5702886, 64), f"./data/delete_rand{32}.txt")

print_data(insertRandom(9227464, 64), f"./data/insert_rand{33}.txt")
print_data(deleteRandom(9227464, 64), f"./data/delete_rand{33}.txt")

print_data(insertRandom(14930351, 64), f"./data/insert_rand{34}.txt")
print_data(deleteRandom(14930351, 64), f"./data/delete_rand{34}.txt")

print_data(insertRandom(24157816, 64), f"./data/insert_rand{35}.txt")
print_data(deleteRandom(24157816, 64), f"./data/delete_rand{35}.txt")
