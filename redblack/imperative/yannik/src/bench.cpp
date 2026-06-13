#include "rbt.h"
#include "bench_helper.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <set>


double getTime() {
    auto t = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(t.time_since_epoch()).count();
}

// largest key = (rightmost path in BST)
int32_t treeMax(Tree* t) {
    while (t->right != nullptr) t = t->right;
    return t->mark;
}

std::pair<std::vector<double>, std::vector<double>> benchInsertClear(int n, int reps) {
    Tree tree;
    std::vector<long> inserts = getFullInserts(n);
    std::vector<double> times;
    std::vector<double> memory;
    for (int t = 0; t < reps; t++) {
        Tree::bytes_allocated = 0;
        auto t0 = getTime();
        for (int i = 0; i < (int)inserts.size(); i++) {
            tree.insert(inserts[i]);
        }
        times.push_back((getTime() - t0) / inserts.size());
        memory.push_back((double) Tree::bytes_allocated / inserts.size());
        tree.free();
    }
    return {times, memory};
}

std::pair<std::vector<double>, std::vector<double>> benchDeleteClear(int n, int reps) {
    Tree tree;
    std::vector<long> inserts = getFullInserts(n);
    std::vector<long> deletes = getFullDeletes(n);
    std::vector<double> times;
    std::vector<double> memory;
    for (int t = 0; t < reps; t++) {
        for (int i = 0; i < (int)inserts.size(); i++) tree.insert(inserts[i]);
        Tree::bytes_allocated = 0;
        auto t0 = getTime();
        for (int i = 0; i < (int)deletes.size(); i++) {
            tree.delete_node(deletes[i]);
        }
        times.push_back((getTime() - t0) / deletes.size());
        memory.push_back((double) Tree::bytes_allocated / deletes.size());
        tree.free();
    }
    return {times, memory};
}

// run the mixed workload from a range_*.txt file
// each line = one block of ~10000 commands, tree persists between blocks
// same setup as the OCaml benchmark
void benchMixed(const char* path, int depth, bool balanced, int reps, const std::string& label, std::ofstream& out) {
    std::vector<double> times;
    std::vector<double> memory;
    Tree tree;
    for (int i = 0; i < reps; i++) {
        if (balanced) makeBalanced(depth, tree);
        else makeUnbalanced(depth, tree);
        if (i == 0) std::cout << "Number of nodes: " << treeMax(&tree) / 64 << "\n";
        //validate(*tree);

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "could not open " << path << "\n";
            return;
        }

        std::string line;
        int index = 0;
        while (std::getline(file, line)) {
            int cmdCount = 0;
            int duplicates = 0;
            auto cmds = parseLine(line);
            //resetMem(version);
            Tree::bytes_allocated = 0;
            double t0 = getTime();
            for (auto& cmd : cmds) {
                cmdCount++;
                //std::cout << cmd.type << ", " << cmd.key << "\n";
                if      (cmd.type == 'i') tree.insert(cmd.key);
                else if (cmd.type == 'd') tree.delete_node(cmd.key);
                else                      tree.lookup(cmd.key);
            }
            times.push_back((getTime() - t0) / cmdCount);
            memory.push_back((double) Tree::bytes_allocated / cmdCount);
            //memory.push_back((double) getMem(version) / cmdCount);
            //std::cout << "Duplicates: " << duplicates << "/" << cmdCount << "\n";
        }
        file.close();
        tree.free();
    }
    writeResult(label, times, memory, out);
}

// same depths and files as OCaml benchmark
const int  BALANCED_DEPTHS[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
const char* BALANCED_PATHS[] = {
    "../../../../avl/avl-functional/data/range_32767.txt",
    "../../../../avl/avl-functional/data/range_65535.txt",
    "../../../../avl/avl-functional/data/range_131071.txt",
    "../../../../avl/avl-functional/data/range_262143.txt",
    "../../../../avl/avl-functional/data/range_524287.txt",
    "../../../../avl/avl-functional/data/range_1048575.txt",
    "../../../../avl/avl-functional/data/range_2097151.txt",
    "../../../../avl/avl-functional/data/range_4194303.txt",
    "../../../../avl/avl-functional/data/range_8388607.txt",
    "../../../../avl/avl-functional/data/range_16777215.txt",
    "../../../../avl/avl-functional/data/range_33554431.txt",
};

const int  UNBALANCED_DEPTHS[] = {10, 11, 12, 13, 14, 15, 16};
const char* UNBALANCED_PATHS[] = {
"../../../../avl/avl-functional/data/range_32767.txt",
"../../../../avl/avl-functional/data/range_121392.txt",
"../../../../avl/avl-functional/data/range_317810.txt",
"../../../../avl/avl-functional/data/range_1048575.txt",
"../../../../avl/avl-functional/data/range_3524577.txt",
"../../../../avl/avl-functional/data/range_9227464.txt",
"../../../../avl/avl-functional/data/range_24157816.txt"
};



/*
    BenchMixed: time in seconds for 10000 operations, alternating between inserts and deletes
    Worst/Best/Amortized: Average time in seconds for 1 operation
*/

int main() {

    // mixed workload: compact RBT, same range_*.txt files as OCaml benchmark
    {
        std::ofstream compOut("amortized_cpp_opt_long.txt");

        std::cout << "insert amortized cost...\n";
        
        for (int n = 1; n <= 20; n++) {
            auto [times, bytes] = benchInsertClear(n * 50000, 10);
            writeResult("i_amort_" + std::to_string(n * 50000), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        
    
        std::cout << "delete amortized cost...\n";
        for (int n = 1; n <= 20; n++) {
            auto [times, bytes] = benchDeleteClear(n * 50000, 10);
            writeResult("d_amort_" + std::to_string(n * 50000), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        compOut.close();
        std::ofstream mixedOut("result_cpp_opt_long.txt");

        for (int i = 0; i < 11; i++) {
            int n = BALANCED_DEPTHS[i];
            std::cout << "balanced depth " << n << "...\n";
            benchMixed(BALANCED_PATHS[i], n, true, 10, "best_" + std::to_string(n), mixedOut);
        }

        
        for (int i = 0; i < 7; i++) {
            int n = UNBALANCED_DEPTHS[i];
            std::cout << "Unbalanced depth: " << n << "...\n";
            benchMixed(UNBALANCED_PATHS[i], n, false, 10, "worst_" + std::to_string(n), mixedOut);
        }
        
        mixedOut.close();

    }

    std::cout << "done! results in results_comp_cpp.txt and mixed_cpp_compact.txt\n";
    return 0;
}