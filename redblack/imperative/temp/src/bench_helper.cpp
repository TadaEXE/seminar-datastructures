#include "bench_helper.h"



Node* buildBalancedHelper(int minKey, int step, int depth, Node* nil, Node* parent) {
    if (depth <= 0) return nil;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    auto *root = new Node{
        .parent = parent,
        .left = nil,
        .right = nil,
        .data = std::move(rootKey),
        .color = Node::Color::Black,
    };
    root->left  = buildBalancedHelper(minKey,   step, depth - 1, nil, root);
    root->right = buildBalancedHelper(rootKey,  step, depth - 1, nil, root);
    return root;
}

Node* buildUnbalancedHelper(int& nextKey, int step, int depth, Node::Color color, Node* nil, Node* parent) {
    if (depth <= 0) return nil;
    Node::Color leftColor = color == Node::Color::Black ? Node::Color::Red : Node::Color::Black;
    int childDepth = color == Node::Color::Black ? depth - 1 : depth;
    Node* left = buildUnbalancedHelper(nextKey, step, childDepth, leftColor, nil, nil);
    auto *root = new Node{
        .parent = parent,
        .left = nil,
        .right = nil,
        .data = std::move(nextKey),
        .color = color,
    };
    left->parent = root;
    nextKey += step;
    root->left  = left;
    //The right child is always black since it is the shorter path
    root->right = buildUnbalancedHelper(nextKey, step, childDepth, Node::Color::Black, nil, root);
    return root;
}

Tree* makeBalanced(int depth) {
    auto tree = new Tree();
    auto root = buildBalancedHelper(0, 64, depth, &tree->_nil, &tree->_nil);
    tree->_root = root;
    return tree;
}

Tree* makeUnbalanced(int depth) {
    auto tree = new Tree();
    int nextKey = 0;
    auto root = buildUnbalancedHelper(nextKey, 64, depth, Node::Color::Black, &tree->_nil, &tree->_nil);
    tree->_root = root;
    return tree;
}

/*
std::vector<int> getFullDeletes(int n) {
    std::srand(0);
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
    return deletes;
}

std::vector<int> getFullInserts(int n) {
    std::srand(0);
    std::vector<int> inserts;
    for (int i = 0; i < n; i++) {
        int random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
    }
    return inserts;
}
    */


