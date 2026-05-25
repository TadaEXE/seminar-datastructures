#pragma once
#include <utility>
#include <climits>

struct Node {
    int key;
    int height;
    Node* left;
    Node* right;

    Node(int k) : key(k), height(1), left(nullptr), right(nullptr) {}
};

int height(Node* n);
void updateHeight(Node* n);

Node* balL(Node* z);
Node* balR(Node* z);

bool search(Node* n, int key);
Node* insert(Node* n, int key);

std::pair<Node*, int> split_max(Node* t);
Node* deleteNode(Node* n, int key);

void inorder(Node* n);
void freeTree(Node* n);

// invariant checks
bool avl_inv(Node* n);
bool search_tree_inv(Node* n, long long min = LLONG_MIN, long long max = LLONG_MAX);
