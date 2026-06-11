#include "rbt.hpp"
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

void validate_red_rule(Node *node, Node *nil) {
  if (node == nil)
    return;

  if (node->color == Node::Color::Red) {
    assert(node->left->color == Node::Color::Black);

    assert(node->right->color == Node::Color::Black);
  }

  validate_red_rule(node->left, nil);

  validate_red_rule(node->right, nil);
}

void validate(Tree &tree) {
  auto root = tree.root();
  auto nil = tree.nil();
  if (root == nil)
    return;

  assert(root->color == Node::Color::Black);
  std::cout << "X1: " << tree._root->left->data << ", " << tree._root->right->data << "\n";
  validate_red_rule(root, nil);
  std::cout << "X2: " << tree._root << ", " << tree._root->left << ", " << tree._root->right << "\n";
  tree.black_height(root);
  std::vector<int32_t> values;

  std::cout << "X3: " << tree._root << ", " << tree._root->left << ", " << tree._root->right << "\n";
  tree.inorder(root, &values);
  std::cout << "X4: " << tree._root << ", " << tree._root->left << ", " << tree._root->right << "\n";
  for (size_t i = 1; i < values.size(); ++i) {
    assert(values[i - 1] < values[i]);
  }
}

double getTime() {
    auto t = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(t.time_since_epoch()).count();
}

// largest key = (rightmost path in BST)
int32_t treeMax(Node* t, Node* nil) {
    while (t->right != nil) t = t->right;
    return t->data;
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
void benchMixed(const char* path, int depth, bool balanced, int reps, const std::string& label, std::ofstream& out) {
    std::vector<double> times;
    std::vector<double> memory;
    std::cout << "1\n";
    Tree* tree;
    std::cout << "2\n";
    for (int i = 0; i < reps; i++) {
        if (balanced) tree = makeBalanced(depth);
        else tree = makeUnbalanced(depth); 
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
            double t0 = getTime();
            std::cout << "Start Line: " << index++ <<"\n";
            for (auto& cmd : cmds) {
                cmdCount++;
                //std::cout << cmd.type << ", " << cmd.key << "\n";
                if (index == 6 && cmdCount == 3673) std::cout << cmd.type << ", " << cmd.key << "\n";
                if      (cmd.type == 'i') tree->insert(cmd.key);
                else if (cmd.type == 'd') tree->remove(cmd.key);
                else                      tree->find(cmd.key);
                if (index == 6 && cmdCount > 3670 && cmdCount < 3700) std::cout << cmdCount << "\n";
                if (index == 6 && cmdCount == 3672 || cmdCount == 3673) std::cout << tree->_root->left->data << ", " << tree->_root->right->data << "\n";
                if (index == 6 && cmdCount == 3672) validate(*tree);
                if (index == 6 && cmdCount == 3672 || cmdCount == 3673) std::cout << tree->_root->left->data << ", " << tree->_root->right->data << "\n";
                if (index == 6 && cmdCount == 3672) std::cout << "Validated\n";
            }
            //validate(*tree);
            std::cout << "Done\n";
            times.push_back((getTime() - t0) / cmdCount);
            memory.push_back(0);
            //memory.push_back((double) getMem(version) / cmdCount);
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
    "../../../avl/avl-functional/data/range_32767.txt",
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

const int  UNBALANCED_DEPTHS[] = {6, 7, 8, 9, 10, 11, 12};
const char* UNBALANCED_PATHS[] = {
"../../../avl/avl-functional/data/range_32767.txt",
"../avl-functional/data/range_75024.txt",
"../avl-functional/data/range_262143.txt",
"../avl-functional/data/range_832039.txt",
"../avl-functional/data/range_2097151.txt",
"../avl-functional/data/range_8388607.txt",
"../avl-functional/data/range_24157816.txt"
};



/*
    BenchMixed: time in seconds for 10000 operations, alternating between inserts and deletes
    Worst/Best/Amortized: Average time in seconds for 1 operation
*/

int main() {

    // mixed workload: compact RBT, same range_*.txt files as OCaml benchmark
    {

        std::ofstream mixedOut("mixed_cpp_opt.txt");
        for (int i = 0; i < 11; i++) {
            int n = BALANCED_DEPTHS[i];
            std::cout << "balanced depth " << n << "...\n";
            benchMixed(BALANCED_PATHS[i], n, true, 10, "best_" + std::to_string(n), mixedOut);
        }

        for (int i = 0; i < 7; i++) {
            int n = UNBALANCED_DEPTHS[i];
            std::cout << "Start constructing\n";
            auto tree = makeUnbalanced(n); 
            std::cout << "Constructed\n";
            std::cout << "unbalanced depth " << n << ", " << treeMax(tree->_root, &tree->_nil) << "...\n";
            benchMixed(UNBALANCED_PATHS[i], n, false, 10, "worst_" + std::to_string(n), mixedOut);
        }
        mixedOut.close();

    }

    std::cout << "done! results in results_comp_cpp.txt and mixed_cpp_compact.txt\n";
    return 0;
}