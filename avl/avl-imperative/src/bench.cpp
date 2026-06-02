#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "avl.h"

// current time in seconds
double getTime() {
    auto t = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(t.time_since_epoch()).count();
}

// build a left-heavy AVL tree
// at every node: left subtree has depth-1, right subtree has depth-2
// so the leftmost path is much longer than the rightmost
Node* buildUnbalancedHelper(int& nextKey, int step, int depth) {
    if (depth <= 0) return nullptr;
    Node* left = buildUnbalancedHelper(nextKey, step, depth - 1);
    Node* root = new Node(nextKey);
    nextKey += step;
    root->left  = left;
    root->right = buildUnbalancedHelper(nextKey, step, depth - 2);
    root->height = 1 + std::max(height(root->left), height(root->right));
    return root;
}

Node* buildUnbalanced(int step, int depth) {
    int nextKey = step; // start from step so all keys are multiples of step
    return buildUnbalancedHelper(nextKey, step, depth);
}

Node* buildBalanced(int minKey, int step, int depth) {
    if (depth <= 0) return nullptr;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    Node* root = new Node(rootKey);
    root->left  = buildBalanced(minKey,   step, depth - 1);
    root->right = buildBalanced(rootKey,  step, depth - 1);
    root->height = depth; // perfectly balanced: height = depth
    return root;
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
void writeResult(const std::string& name, const std::vector<double>& times, std::ofstream& out) {
    out << "\"" << name << "\":[";
    for (int i = 0; i < (int)times.size(); i++) {
        out << std::fixed << std::setprecision(9)
            << "(" << times[i] << ", 0.0)";
        if (i + 1 < (int)times.size()) out << ", ";
    }
    out << "],\n";
    out.flush();
}

// C++ trees are mutable so we can't repeat the same insert like OCaml does
// Instead we insert K keys in the same direction and divide by K
std::vector<double> benchInsert(int depth, int reps, bool worst) {
    const int K = 10000;
    std::vector<double> times;
    for (int i = 0; i < reps; i++) {
        Node* tree = buildUnbalanced(64, depth);
        int x0   = worst ? treeMin(tree) - 64 : treeMax(tree) + 64;
        int step = worst ? -64 : 64;

        double t0 = getTime();
        for (int k = 0; k < K; k++)
            tree = insert(tree, x0 + k * step);
        times.push_back((getTime() - t0) / K);

        freeTree(tree);
    }
    return times;
}

// preinsert K elements, then measure K deletes
// best case: delete from the right side (short traversal, right is shallower)
// worst case: delete from the left side (long traversal, left is deeper)
std::vector<double> benchDelete(int depth, int reps, bool worst) {
    const int K = 10000;
    std::vector<double> times;
    for (int i = 0; i < reps; i++) {
        Node* tree = buildUnbalanced(64, depth);
        int x0   = worst ? treeMin(tree) - 64 : treeMax(tree) + 64;
        int step = worst ? -64 : 64;

        // preinsert so we have K elements to delete
        for (int k = 0; k < K; k++)
            tree = insert(tree, x0 + k * step);

        double t0 = getTime();
        for (int k = 0; k < K; k++)
            tree = deleteNode(tree, x0 + k * step);
        times.push_back((getTime() - t0) / K);

        freeTree(tree);
    }
    return times;
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
void benchMixed(const char* path, Node* tree, const std::string& label, std::ofstream& out) {
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
            if      (cmd.type == 'i') tree = insert(tree, cmd.key);
            else if (cmd.type == 'd') tree = deleteNode(tree, cmd.key);
            else                      search(tree, cmd.key);
        }
        times.push_back(getTime() - t0);
    }

    writeResult(label, times, out);
    freeTree(tree);
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

int main() {
    std::ofstream compOut("results_comp_cpp.txt");

    std::cout << "insert best case...\n";
    for (int n = 15; n <= 34; n++) {
        writeResult("i_opt_" + std::to_string(n), benchInsert(n, 5, false), compOut);
        std::cout << "  n=" << n << "\n";
    }

    std::cout << "insert worst case...\n";
    for (int n = 15; n <= 34; n++) {
        writeResult("i_worst_" + std::to_string(n), benchInsert(n, 5, true), compOut);
        std::cout << "  n=" << n << "\n";
    }

    std::cout << "delete best case...\n";
    for (int n = 15; n <= 34; n++) {
        writeResult("d_opt" + std::to_string(n), benchDelete(n, 5, false), compOut);
        std::cout << "  n=" << n << "\n";
    }

    std::cout << "delete worst case...\n";
    for (int n = 15; n <= 34; n++) {
        writeResult("d_worst" + std::to_string(n), benchDelete(n, 5, true), compOut);
        std::cout << "  n=" << n << "\n";
    }

    compOut.close();

    // mixed workload: same range_*.txt files as OCaml 
    std::ofstream mixedOut("results_mixed_cpp.txt");

    for (int i = 0; i < 11; i++) {
        int n = DEPTHS[i];
        std::cout << "mixed workload depth " << n << "...\n";
        Node* tree = buildBalanced(0, 64, n);
        benchMixed(PATHS[i], tree, "cpp_" + std::to_string(n), mixedOut);
    }

    mixedOut.close();

    std::cout << "done! results in results_comp_cpp.txt and results_mixed_cpp.txt\n";
    return 0;
}
