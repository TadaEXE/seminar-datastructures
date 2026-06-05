#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "avl_iterative.h"
#include "avl_vector.h"
#include "avl_compact.h"

using namespace avl;

Node* constructBalanced(int n, int min, int step) {
    //min + (pow2 (depth - 1)) * step
    std::vector<Node*> lowestLevel;
    int index;
    Node* root = new Node((std::pow(2, n - 1) - 1) * step + min);
    root->height = n;
    lowestLevel.push_back(root);
    std::vector<Node*> newLevel;
    for (int i = 1; i < n; i++) {
        unsigned int level_start = (std::pow(2, (n - i - 1)) - 1) * step + min;
        unsigned int level_step = (1 << (n - i)) * step;
        for (int j = 0; j < std::pow(2, i); j++) {
            Node* added = new Node(level_start + j * level_step);
            added->height = n - i;
            newLevel.push_back(added);
            if (j % 2 == 0) {
                lowestLevel[j / 2]->left = added;
            } else {
                lowestLevel[j / 2]->right = added;
            }
        }
        lowestLevel = newLevel;
        newLevel.clear();
    }
    return root;
}

uint64_t constructBalancedCompact(int n, int min, int step) {
    //min + (pow2 (depth - 1)) * step
    std::vector<uint64_t> lowestLevel;
    int index;
    Node* root = new Node((std::pow(2, n - 1) - 1) * step + min);
    root->height = n;
    lowestLevel.push_back((uint64_t) root);
    std::vector<uint64_t> newLevel;
    for (int i = 1; i < n; i++) {
        unsigned int level_start = (std::pow(2, (n - i - 1)) - 1) * step + min;
        unsigned int level_step = (1 << (n - i)) * step;
        for (int j = 0; j < std::pow(2, i); j++) {
            Node* added = new Node(level_start + j * level_step);
            added->height = n - i;
            newLevel.push_back((uint64_t) added);
            if (j % 2 == 0) {
                ((CompactNode*) lowestLevel[j / 2])->left = (uint64_t) added;
            } else {
                ((CompactNode*) lowestLevel[j / 2])->right = (uint64_t) added;
            }
        }
        lowestLevel = newLevel;
        newLevel.clear();
    }
    return (uint64_t) root;
}

void constructBalancedVector(int n, int min, int step, std::vector<VectorNode> *nodes) {
    nodes->resize(2 << n);
    std::vector<int> lowestLevel;
    Node* root = new Node((std::pow(2, n - 1) - 1) * step + min);
    root->height = n;
    lowestLevel.push_back(0);
    std::vector<int> newLevel;
    int index = 0;
    for (int i = 1; i < n; i++) {
        unsigned int level_start = (std::pow(2, (n - i - 1)) - 1) * step + min;
        unsigned int level_step = (1 << (n - i)) * step;
        for (int j = 0; j < std::pow(2, i); j++) {
            VectorNode node;
            node.key = level_start + j * level_step;
            node.height = n - i;
            node.left = -1;
            node.right = -1;
            newLevel.push_back(index);
            (*nodes)[index] = node;
            if (j % 2 == 0) {
                nodes->at(lowestLevel[j / 2]).left = index;
            } else {
                nodes->at(lowestLevel[j / 2]).right = index;
            }
            index++;
        }
        lowestLevel = newLevel;
        newLevel.clear();
    }
}

std::vector<int> nextSequence(std::ifstream *file) {
    std::vector<int> sequence;
    std::string str;
    if (std::getline(*file, str))
    {
        int start = 1;
        for (int i = 1; i < str.length(); i++) {
            if (str[i] == 'i' || str[i] == 'd' || str[i] == 'f') {
                sequence.push_back(std::stoi(str.substr(start, (i - start))));
                start = i + 1;
            }
        }
        sequence.push_back(std::stoi(str.substr(start, str.length() - start)));
    }
    return sequence;
} 

void benchmark(char* path, Avl* avl) {
    assert(avl->checkInv());
    bool isInsertion = true;
    std::ifstream file(path);
    std::vector<int> sequence = nextSequence(&file);
    int count = 0;
    int insSum = 0;
    int delSum = 0;
    while (sequence.size() > 0) {
        std::vector<void*> stuff;
        if (isInsertion) {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                avl->ins(sequence[i]);
            }
            auto end = clock();
            insSum += end - start;
            //std::cout << "Insertion time: " << end - start << "\n";
        } else {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                avl->del(sequence[i]);
            }
            auto end = clock();
            delSum += end - start;
            //std::cout << "Deletion time: " << end - start << "\n";
        }
        isInsertion = !isInsertion;
        sequence = nextSequence(&file);
        count++;
    }
    std::cout << "Average insert: " << insSum / count * 2 << "\n";
    std::cout << "Average delete: " << delSum / count * 2 << "\n";
    assert(avl->checkInv());
}

void benchmarkRec(char* path, Node* root) {
    bool isInsertion = true;
    std::ifstream file(path);
    std::vector<int> sequence = nextSequence(&file);
    int count = 0;
    int insSum = 0;
    int delSum = 0;
    while (sequence.size() > 0) {
        if (isInsertion) {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                root = insert(root, sequence[i]);
            }
            auto end = clock();
            insSum+=end - start;
            //std::cout << "Insertion time: " << end - start << "\n";
        } else {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                root = deleteNode(root, sequence[i]);
            }
            auto end = clock();
            delSum+=end-start;
            //std::cout << "Deletion time: " << end - start << "\n";
        }
        isInsertion = !isInsertion;
        sequence = nextSequence(&file);
        count++;
    }
    std::cout << "Average insert: " << insSum / count * 2 << "\n";
    std::cout << "Average delete: " << delSum / count * 2 << "\n";
    assert(avl_inv(root));
}

void clearBenchmark(int n) {
    std::srand(0); 
    AvlVec avlVec;
    AvlIt avlIt;
    AvlComp avlComp;
    Node* AvlIt = new Node(0);
    std::vector<int> inserts;
    std::vector<int> insertsTmp;
    std::vector<int> deletes;
    for (int i = 0; i < n; i++) {
        int random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
        insertsTmp.push_back(random_value);
    }
    for (int i = 0; i < insertsTmp.size(); i++) {
        int random_value = std::rand() % insertsTmp.size();
        deletes.push_back(insertsTmp[random_value]);
        insertsTmp.erase(insertsTmp.begin() + random_value);
        if (insertsTmp.empty()) break;
    }
    std::cout << "\nRecursive: " << n << "\n";
    auto t0 = clock();
    for (int i = 0; i < inserts.size(); i++) {
        AvlIt = insert(AvlIt, inserts[i]);
    }
    auto t1 = clock();
    std::cout << "Insert: " << t1 - t0 << "\n";
    for (int i = 0; i < deletes.size(); i++) {
        AvlIt = deleteNode(AvlIt, deletes[i]);
    }
    auto t2 = clock();
    std::cout << "Remove: " << t2 - t1 << "\n";

    std::cout << "\nIterative: " << n << "\n";
    auto t3 = clock();
    for (int i = 0; i < inserts.size(); i++) {
        avlIt.ins(inserts[i]);
    }
    auto t4 = clock();
    std::cout << "Insert: " << t4 - t3 << "\n";
    for (int i = 0; i < deletes.size(); i++) {
        avlIt.del(inserts[i]);
    }
    auto t5 = clock();
    std::cout << "Remove: " << t5 - t4 << "\n";

    std::cout << "\nVector: " << n << "\n";
    auto t6 = clock();
    for (int i = 0; i < inserts.size(); i++) {
        avlVec.ins(inserts[i]);
    }
    auto t7 = clock();
    std::cout << "Insert: " << t7 - t6 << "\n";
    for (int i = 0; i < deletes.size(); i++) {
        avlVec.del(deletes[i]);
    }
    auto t8 = clock();
    std::cout << "Remove: " << t8 - t7 << "\n";

    std::cout << "\nCompact: " << n << "\n";
    auto t9 = clock();
    for (int i = 0; i < inserts.size(); i++) {
        avlComp.ins(inserts[i]);
    }
    auto t10 = clock();
    std::cout << "Insert: " << t10 - t9 << "\n";
    for (int i = 0; i < deletes.size(); i++) {
        avlComp.del(deletes[i]);
    }
    auto t11 = clock();
    std::cout << "Remove: " << t11 - t10 << "\n";
}

int main() 
{
    char* paths[] = {
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
    for (int n = 0; n < 11; n++) {
        /*
        std::cout << "Compact: " << n << "\n";
        uint64_t root4 = constructBalancedCompact(n + 15, 0, 64);
        benchmark(paths[n], new AvlComp(root4, n + 15));
        */
        
        std::cout << "Recursive: " << n << "\n";
        Node* root2 = constructBalanced(n + 15, 0, 64);
        benchmarkRec(paths[n], root2);

        std::cout << "Iterative: " << n << "\n";
        Node* root1 = constructBalanced(n + 15, 0, 64);
        benchmark(paths[n], new AvlIt(root1));
        
        /*
        std::cout << "Vector: \n";
        std::vector<VectorNode> nodes;
        constructBalancedVector(n + 15, 0, 64, &nodes);
        auto avl = new AvlVec(&nodes);
        //Avoid starting with resize
        benchmark(paths[n], avl);
        avl->printLimit();
        */
    }

}

int tmp() {
    for (int n = 0; n < 10; n++) {
        clearBenchmark((n + 1) * 50000);
    }
}
