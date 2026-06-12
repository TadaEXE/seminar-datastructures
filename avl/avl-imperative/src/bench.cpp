#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include "avl.h"
#include "bench_helpers.h"
#include <assert.h>

// current time in seconds
double getTime() {
    auto t = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(t.time_since_epoch()).count();
}

// smallest key = (leftmost path in BST)
int treeMin(Node* t) {
    while (t->left) t = t->left;
    return t->key;
}

// largest key = (rightmost path in BST)
int treeMax(Node* t) {
    while (t->right) t = t->right;
    return t->key;
}

void resetMem(int version) {
    if (version == 0 || version == 1 || version == 4) Node::bytes_allocated = 0;
    else if (version == 2) VectorNode::bytes_allocated = 0;
    else CompactNode::bytes_allocated = 0;
}

long long getMem(int version) {
    if (version == 0 || version == 1 || version == 4) return Node::bytes_allocated;
    else if (version == 2) return VectorNode::bytes_allocated;
    else return CompactNode::bytes_allocated;
}

// write results in the same format as OCaml's plots.py data_2 dict
// second value is memory-related: C++ uses estimated bytes per operation.
void writeResult(const std::string& name, const std::vector<double>& times,
                 const std::vector<double>& memory, std::ofstream& out) {
    out << "\"" << name << "\":[";
    for (int i = 0; i < (int)times.size(); i++) {
        out << std::fixed << std::setprecision(9)
            << "(" << times[i] << ", " << memory[i] << ")";
        if (i + 1 < (int)times.size()) out << ", ";
    }
    out << "],\n";
    out.flush();
}

std::pair<std::vector<double>, std::vector<double>> benchInsertClear(int n, int reps, int version) {
    Avl* avl;
    if (version == 0) avl = new AvlRec();
    else if (version == 1) avl = new AvlIt();
    else if (version == 2) avl = new AvlVec();
    else if (version == 3)  avl = new AvlComp();
    else avl = new AvlDum();
    std::vector<long> inserts = getFullInserts(n);
    std::vector<double> times;
    std::vector<double> memory;
    for (int t = 0; t < reps; t++) {
        resetMem(version);
        auto t0 = getTime();
        for (size_t i = 0; i < inserts.size(); i++) {
            avl->ins(inserts[i]);
        }
        times.push_back((getTime() - t0) / inserts.size());
        memory.push_back((double) getMem(version) / inserts.size());
        avl->free();
    }
    delete avl;
    return {times, memory};
}

std::pair<std::vector<double>, std::vector<double>> benchDeleteClear(int n, int reps, int version) {
    Avl* avl;
    if (version == 0) avl = new AvlRec();
    else if (version == 1) avl = new AvlIt();
    else if (version == 2) avl = new AvlVec();
    else if (version == 3) avl = new AvlComp();
    else avl = new AvlDum();
    std::vector<long> inserts = getFullInserts(n);
    std::vector<long> deletes = getFullDeletes(n);
    std::vector<double> times;
    std::vector<double> memory;
    for (int t = 0; t < reps; t++) {
        for (size_t i = 0; i < inserts.size(); i++) avl->ins(inserts[i]);
        resetMem(version);
        auto t0 = getTime();
        for (size_t i = 0; i < deletes.size(); i++) {
            avl->del(deletes[i]);
        }
        times.push_back((getTime() - t0) / deletes.size());
        memory.push_back((double) getMem(version) / deletes.size());
        avl->free();
        resetMem(version);
    }
    delete avl;
    return {times, memory};
}

void writeResult(const std::string& name, const std::vector<double>& times, std::ofstream& out) {
    std::vector<double> memory;
    memory.resize(times.size());
    for (int i = 0; i < times.size(); i++) memory[i] = 0;
    writeResult(name, times, memory, out);
}

// C++ trees are mutable so we can't repeat the same insert like OCaml does
// Instead we insert K keys in the same direction and divide by K
std::pair<std::vector<double>, std::vector<double>> benchInsert(int depth, int reps, bool worst, int version) {
    const int K = 10000;
    std::vector<double> times;
    std::vector<double> memory;
    for (int i = 0; i < reps; i++) {
        Avl* tree = makeUnbalanced(depth, version);
        int x0   = worst ? tree->min() - 64 : tree->max() + 64;
        int step = worst ? -64 : 64;

        resetMem(version);
        double t0 = getTime();
        for (int k = 0; k < K; k++)
            tree->ins(x0 + k * step);
        times.push_back((getTime() - t0) / K);
        memory.push_back(getMem(version) / K);
        tree->free();
    }
    return {times, memory};
}

// preinsert K elements, then measure K deletes
// best case: delete from the right side (short traversal, right is shallower)
// worst case: delete from the left side (long traversal, left is deeper)
std::pair<std::vector<double>, std::vector<double>> benchDelete(int depth, int reps, bool worst, int version) {
    const int K = 10000;
    std::vector<double> times;
    std::vector<double> memory;
    for (int i = 0; i < reps; i++) {
        Avl* tree = makeUnbalanced(depth, version);
        assert(tree->checkInv());

        int x0   = worst ? tree->max() : tree->min();
        int step = worst ? -64 : 64;

        resetMem(version);
        double t0 = getTime();
        for (int k = 0; k < K; k++)
            tree->del(x0 + k * step);
        times.push_back((getTime() - t0) / K);
        memory.push_back(getMem(version) / K);
        tree->free();
    }
    return {times, memory};
}

// parse one line of commands like "i42d-3f17i100..."
struct Cmd { char type; int key; };

std::vector<Cmd> parseLine(const std::string& line) {
    std::vector<Cmd> cmds;
    int i = 0;
    while (i < (int)line.size()) {
        char type = line[i++];
        if (type != 'i' && type != 'd' && type != 'f') break;
        bool neg = (i < (int)line.size() && line[i] == '-');
        if (neg) i++;
        int key = 0;
        while (i < (int)line.size() && std::isdigit(line[i]))
            key = key * 10 + (line[i++] - '0');
        cmds.push_back({type, neg ? -key : key});
    }
    return cmds;
}

// run the mixed workload from a range_*.txt file
// each line = one block of ~10000 commands, tree persists between blocks
// same setup as the OCaml benchmark
void benchMixed(const char* path, int depth, bool balanced, int reps, const std::string& label, std::ofstream& out, int version) {
    std::vector<double> times;
    std::vector<double> memory;
    Avl* tree;
    for (int i = 0; i < reps; i++) {
        if (balanced) tree = makeBalanced(depth, version);
        else tree = makeUnbalanced(depth, version); 

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "could not open " << path << "\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            int cmdCount = 0;
            int duplicates = 0;
            auto cmds = parseLine(line);
            resetMem(version);
            double t0 = getTime();
            for (auto& cmd : cmds) {
                cmdCount++;
                if      (cmd.type == 'i')  {
                    if (tree->find(cmd.key)) duplicates++;
                    tree->ins(cmd.key);
                }
                else if (cmd.type == 'd') tree->del(cmd.key);
                else                      tree->find(cmd.key);
            }
            times.push_back((getTime() - t0) / cmdCount);
            memory.push_back((double) getMem(version) / cmdCount);
            //std::cout << "Duplicates: " << duplicates << "/" << cmdCount << "\n";
        }
        file.close();
        tree->free();
    }

    writeResult(label, times, memory, out);
}

// same depths and files as OCaml benchmark
const int  BALANCED_DEPTHS[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
const char* BALANCED_PATHS[] = {
    "../avl-functional/data/range_32767.txt",
    "../avl-functional/data/range_65535.txt",
    "../avl-functional/data/range_131071.txt",
    "../avl-functional/data/range_262143.txt",
    "../avl-functional/data/range_524287.txt",
    "../avl-functional/data/range_1048575.txt",
    "../avl-functional/data/range_2097151.txt",
    "../avl-functional/data/range_4194303.txt",
    "../avl-functional/data/range_8388607.txt",
    "../avl-functional/data/range_16777215.txt",
    "../avl-functional/data/range_33554431.txt",
};

const int  UNBALANCED_DEPTHS[] = {21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
const char* UNBALANCED_PATHS[] = {
"../avl-functional/data/range_28656.txt",
"../avl-functional/data/range_46367.txt",
"../avl-functional/data/range_75024.txt",
"../avl-functional/data/range_121392.txt",
"../avl-functional/data/range_196417.txt",
"../avl-functional/data/range_317810.txt",
"../avl-functional/data/range_514228.txt",
"../avl-functional/data/range_832039.txt",
"../avl-functional/data/range_1346268.txt",
"../avl-functional/data/range_2178308.txt",
"../avl-functional/data/range_3524577.txt",
"../avl-functional/data/range_5702886.txt",
"../avl-functional/data/range_9227464.txt",
"../avl-functional/data/range_14930351.txt",
"../avl-functional/data/range_24157816.txt",
};



/*
    BenchMixed: time in seconds for 10000 operations, alternating between inserts and deletes
    Worst/Best/Amortized: Average time in seconds for 1 operation
*/

int main() {
    // insert/delete comparison: AvlRec (standard pointer-based) vs OCaml functional
    {   
        
        const int compVersion = 4;
        std::ofstream compOut("amortized_cpp_dummy_long.txt");

        std::cout << "insert amortized cost...\n";
        
        for (int n = 1; n <= 20; n++) {
            auto [times, bytes] = benchInsertClear(n * 50000, 10, compVersion);
            writeResult("i_amort_" + std::to_string(n * 50000), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
    
        std::cout << "delete amortized cost...\n";
        for (int n = 1; n <= 20; n++) {
            auto [times, bytes] = benchDeleteClear(n * 50000, 10, compVersion);
            writeResult("d_amort_" + std::to_string(n * 50000), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        compOut.close();
        
        
    }

    // mixed workload: compact AVL, same range_*.txt files as OCaml benchmark
    {
        const int mixVersion = 4;
        std::ofstream mixedOut("mixed_cpp_dummy_long.txt");
        for (int i = 0; i < 11; i++) {
            int n = BALANCED_DEPTHS[i];
            std::cout << "balanced depth " << n << "...\n";
            benchMixed(BALANCED_PATHS[i], n, true, 10, "best_" + std::to_string(n), mixedOut, mixVersion);
        }

        for (int i = 0; i < 15; i++) {
            int n = UNBALANCED_DEPTHS[i];
            std::cout << "unbalanced depth " << n << "...\n";
            benchMixed(UNBALANCED_PATHS[i], n, false, 10, "worst_" + std::to_string(n), mixedOut, mixVersion);
        }
        mixedOut.close();
    }

    std::cout << "done! results in results_comp_cpp.txt and mixed_cpp_compact.txt\n";
    return 0;
}
