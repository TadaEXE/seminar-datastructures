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

// write results in the same format as OCaml's plots.py data_2 dict
// second value is memory-related: C++ uses estimated bytes per operation.
void writeResult(const std::string& name, const std::vector<double>& times,
                 double memoryPerOperation, std::ofstream& out) {
    out << "\"" << name << "\":[";
    for (int i = 0; i < (int)times.size(); i++) {
        out << std::fixed << std::setprecision(9)
            << "(" << times[i] << ", " << memoryPerOperation << ")";
        if (i + 1 < (int)times.size()) out << ", ";
    }
    out << "],\n";
    out.flush();
}

std::vector<double> benchInsertClear(int n, int reps, int version) {
    Avl* avl;
    if (version == 0) avl = new AvlRec();
    else if (version == 1) avl = new AvlIt();
    else if (version == 2) avl = new AvlVec();
    else avl = new AvlComp();
    std::vector<int> inserts = getFullInserts(n);
    std::vector<double> times;
    for (int t = 0; t < reps; t++) {
        auto t0 = getTime();
        for (int i = 0; i < (int)inserts.size(); i++) {
            avl->ins(inserts[i]);
        }
        times.push_back((getTime() - t0) / inserts.size());
        avl->free();
    }
    delete avl;
    return times;
}

std::vector<double> benchDeleteClear(int n, int reps, int version) {
    Avl* avl;
    if (version == 0) avl = new AvlRec();
    else if (version == 1) avl = new AvlIt();
    else if (version == 2) avl = new AvlVec();
    else avl = new AvlComp();
    std::vector<int> inserts = getFullInserts(n);
    std::vector<int> deletes = getFullDeletes(n);
    std::vector<double> times;
    for (int t = 0; t < reps; t++) {
        for (int i = 0; i < (int)inserts.size(); i++) avl->ins(inserts[i]);
        auto t0 = getTime();
        for (int i = 0; i < (int)deletes.size(); i++) {
            avl->del(deletes[i]);
        }
        times.push_back((getTime() - t0) / inserts.size());
        avl->free();
    }
    delete avl;
    return times;
}

void writeResult(const std::string& name, const std::vector<double>& times, std::ofstream& out) {
    writeResult(name, times, 0.0, out);
}

void resetMem(int version) {
    if (version == 0 || version == 1) Node::bytes_allocated = 0;
    else if (version == 2) VectorNode::bytes_allocated = 0;
    else CompactNode::bytes_allocated = 0;
}

long long getMem(int version) {
    if (version == 0 || version == 1) return Node::bytes_allocated;
    else if (version == 2) return VectorNode::bytes_allocated;
    else return CompactNode::bytes_allocated;
}

// C++ trees are mutable so we can't repeat the same insert like OCaml does
// Instead we insert K keys in the same direction and divide by K
std::pair<std::vector<double>, double> benchInsert(int depth, int reps, bool worst, int version) {
    const int K = 10000;
    std::vector<double> times;
    long long total_bytes = 0;
    for (int i = 0; i < reps; i++) {
        Avl* tree = makeUnbalanced(depth, version);
        int x0   = worst ? tree->min() - 64 : tree->max() + 64;
        int step = worst ? -64 : 64;

        resetMem(version);
        double t0 = getTime();
        for (int k = 0; k < K; k++)
            tree->ins(x0 + k * step);
        times.push_back((getTime() - t0) / K);
        total_bytes += getMem(version);

        tree->free();
    }
    return {times, (double)total_bytes / (reps * K)};
}

// preinsert K elements, then measure K deletes
// best case: delete from the right side (short traversal, right is shallower)
// worst case: delete from the left side (long traversal, left is deeper)
std::pair<std::vector<double>, double> benchDelete(int depth, int reps, bool worst, int version) {
    const int K = 10000;
    std::vector<double> times;
    long long total_bytes = 0;
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
        total_bytes += getMem(version);

        tree->free();
    }
    return {times, (double)total_bytes / (reps * K)};
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
void benchMixed(const char* path, Avl* tree, const std::string& label, std::ofstream& out) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "could not open " << path << "\n";
        return;
    }

    std::string line;
    std::vector<double> times;

    while (std::getline(file, line)) {
        auto cmds = parseLine(line);

        double t0 = getTime();
        for (auto& cmd : cmds) {
            if      (cmd.type == 'i') tree->ins(cmd.key);
            else if (cmd.type == 'd') tree->del(cmd.key);
            else                      tree->find(cmd.key);
        }
        times.push_back(getTime() - t0);
    }

    writeResult(label, times, out);
    tree->free();
}

// same depths and files as OCaml benchmark
const int  DEPTHS[] = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
const char* PATHS[] = {
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

/*
    BenchMixed: time in seconds for 10000 operations, alternating between inserts and deletes
    Worst/Best/Amortized: Average time in seconds for 1 operation
*/

int main() {
    // insert/delete comparison: AvlRec (standard pointer-based) vs OCaml functional
    {
        const int compVersion = 0;
        std::ofstream compOut("results_comp_cpp.txt");

        std::cout << "insert worst case...\n";
        for (int n = 15; n <= 34; n++) {
            auto [times, bytes] = benchInsert(n, 5, true, compVersion);
            writeResult("i_worst_" + std::to_string(n), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        std::cout << "insert best case...\n";
        for (int n = 15; n <= 34; n++) {
            auto [times, bytes] = benchInsert(n, 5, false, compVersion);
            writeResult("i_opt_" + std::to_string(n), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        std::cout << "delete best case...\n";
        for (int n = 15; n <= 34; n++) {
            auto [times, bytes] = benchDelete(n, 5, false, compVersion);
            writeResult("d_opt" + std::to_string(n), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        std::cout << "delete worst case...\n";
        for (int n = 15; n <= 34; n++) {
            auto [times, bytes] = benchDelete(n, 5, true, compVersion);
            writeResult("d_worst" + std::to_string(n), times, bytes, compOut);
            std::cout << "  n=" << n << "\n";
        }
        compOut.close();
    }

    // mixed workload: compact AVL, same range_*.txt files as OCaml benchmark
    {
        const int mixVersion = 3;
        std::ofstream mixedOut("mixed_cpp_compact.txt");
        for (int i = 0; i < 11; i++) {
            int n = DEPTHS[i];
            std::cout << "mixed workload depth " << n << "...\n";
            Avl* tree = makeBalanced(n, mixVersion);
            benchMixed(PATHS[i], tree, "cpp_" + std::to_string(n), mixedOut);
        }
        mixedOut.close();
    }

    std::cout << "done! results in results_comp_cpp.txt and mixed_cpp_compact.txt\n";
    return 0;
}
