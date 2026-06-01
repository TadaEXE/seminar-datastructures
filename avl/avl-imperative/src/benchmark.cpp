#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "avl_iterative.h"

using namespace avl;

Node* constructBalanced(int n, int min, int step) {
    //min + (pow2 (depth - 1)) * step
    std::vector<Node*> lowestLevel;
    Node* root = new Node((std::pow(2, n - 1) - 1) * step + min);
    root->height = n;
    lowestLevel.push_back(root);
    std::vector<Node*> newLevel;
    for (int i = 1; i < n; i++) {
        unsigned int level_start = (std::pow(2, (n - i - 1)) - 1) * step + min;
        unsigned int level_step = 1 << (n - i) * step;
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

void benchmark(char* path, AvlRec* avl) {
    bool isInsertion = true;
    std::ifstream file(path);
    std::vector<int> sequence = nextSequence(&file);
    while (sequence.size() > 0) {
        if (isInsertion) {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                avl->ins(sequence[i]);
            }
            auto end = clock();
            std::cout << "Insertion time: " << end - start << "\n";
        } else {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                avl->del(sequence[i]);
            }
            auto end = clock();
            std::cout << "Deletion time: " << end - start << "\n";
        }
        isInsertion = !isInsertion;
        sequence = nextSequence(&file);
    }
}

void benchmarkRec(char* path, Node* root) {
    bool isInsertion = true;
    std::ifstream file(path);
    std::vector<int> sequence = nextSequence(&file);
    while (sequence.size() > 0) {
        if (isInsertion) {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                root = insert(root, sequence[i]);
            }
            auto end = clock();
            std::cout << "Insertion time: " << end - start << "\n";
        } else {
            auto start = clock();
            for (int i = 0; i < sequence.size(); i++) {
                root = deleteNode(root, sequence[i]);
            }
            auto end = clock();
            std::cout << "Deletion time: " << end - start << "\n";
        }
        isInsertion = !isInsertion;
        sequence = nextSequence(&file);
    }
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
        Node* root = constructBalanced(n + 15, 0, 64);
        std::cout << "Iterative: \n";
        benchmark(paths[n], new AvlRec(root));
        std::cout << "Recursive: \n";
        benchmarkRec(paths[n], root);
    }

}