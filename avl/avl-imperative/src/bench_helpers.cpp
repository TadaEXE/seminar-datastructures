#include "bench_helpers.h"

// build a left-heavy AVL tree
// at every node: left subtree has depth-1, right subtree has depth-2
// so the leftmost path is much longer than the rightmost
Node* buildUnbalancedHelperBasic(int& nextKey, int step, int depth, Node* def) {
    if (depth <= 0) return def;
    Node* left = buildUnbalancedHelperBasic(nextKey, step, depth - 1, def);
    Node* root = new Node(nextKey);
    nextKey += step;
    root->left  = left;
    root->right = buildUnbalancedHelperBasic(nextKey, step, depth - 2, def);
    root->height = 1 + std::max(height(root->left), height(root->right));
    return root;
}

Node* buildUnbalancedBasic(int step, int depth, Node* def) {
    int nextKey = step; // start from step so all keys are multiples of step
    return buildUnbalancedHelperBasic(nextKey, step, depth, def);
}

Node* buildBalancedBasic(int minKey, int step, int depth, Node* def) {
    if (depth <= 0) return def;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    Node* root = new Node(rootKey);
    root->left  = buildBalancedBasic(minKey,   step, depth - 1, def);
    root->right = buildBalancedBasic(rootKey,  step, depth - 1, def);
    root->height = depth; // perfectly balanced: height = depth
    return root;
}

// build a left-heavy AVL tree
// at every node: left subtree has depth-1, right subtree has depth-2
// so the leftmost path is much longer than the rightmost
uint64_t buildUnbalancedHelperCompact(int& nextKey, int step, int depth) {
    if (depth <= 0) return 0;
    uint64_t left = buildUnbalancedHelperCompact(nextKey, step, depth - 1);
    CompactNode* root = new CompactNode(nextKey);
    nextKey += step;
    root->left  = left;
    root->right = buildUnbalancedHelperCompact(nextKey, step, depth - 2);
    return SET_BALANCE(root, depth > 1 ? 2 : 0);
}

uint64_t buildUnbalancedCompact(int step, int depth) {
    int nextKey = step; // start from step so all keys are multiples of step
    return buildUnbalancedHelperCompact(nextKey, step, depth);
}

uint64_t buildBalancedCompact(int minKey, int step, int depth) {
    if (depth <= 0) return 0;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    CompactNode* root = new CompactNode(rootKey);
    root->left  = buildBalancedCompact(minKey,   step, depth - 1);
    root->right = buildBalancedCompact(rootKey,  step, depth - 1);
    return (uint64_t) root;
}

// build a left-heavy AVL tree
// at every node: left subtree has depth-1, right subtree has depth-2
// so the leftmost path is much longer than the rightmost
int buildUnbalancedHelperVector(int& nextKey, int step, int depth, std::vector<VectorNode> *vector) {
    if (depth <= 0) return -1;
    int rootIndex = vector->size();
    vector->resize(rootIndex + 1);
    int left = buildUnbalancedHelperVector(nextKey, step, depth - 1, vector);
    vector->at(rootIndex).left = left;
    nextKey += step;
    vector->at(rootIndex).key = nextKey;
    vector->at(rootIndex).height = depth;
    int right = buildUnbalancedHelperVector(nextKey, step, depth - 2, vector);
    vector->at(rootIndex).right = right;
    int depthLeft = left >= 0 ? vector->at(left).height : 0;
    int depthRight = right >= 0 ? vector->at(right).height : 0;
    return rootIndex;
}

int buildUnbalancedVector(int step, int depth, std::vector<VectorNode> *vector) {
    int nextKey = step; // start from step so all keys are multiples of step
    return buildUnbalancedHelperVector(nextKey, step, depth, vector);
}

int buildBalancedVector(int minKey, int step, int depth, std::vector<VectorNode> *vector) {
    if (depth <= 0) return -1;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    int rootIndex = vector->size();
    vector->resize(rootIndex + 1);
    vector->at(rootIndex).key = rootKey;
    vector->at(rootIndex).height = depth;
    vector->at(rootIndex).left  = buildBalancedVector(minKey,   step, depth - 1, vector);
    vector->at(rootIndex).right = buildBalancedVector(rootKey,  step, depth - 1, vector);
    return rootIndex;
}

Avl* makeBalanced(int depth, int version) {
    if (version == 0) {
        return new AvlRec(buildBalancedBasic(0, 64, depth, nullptr));
    } else if (version == 1) {
        return new AvlIt(buildBalancedBasic(0, 64, depth, nullptr));
    } else if (version == 2) {
        //This leaks the memory of an empty vector, but whatever
        auto avl = new AvlVec();
        buildBalancedVector(0, 64, depth, avl->getNodes());
        avl->setSize();
        return avl;
    } else if (version == 3) {
        return new AvlComp(buildBalancedCompact(0, 64, depth), depth);
    } else if (version == 4) {
        Node* nil = new Node();
        return new AvlDum(buildBalancedBasic(0, 64, depth, nil), nil);
    }
    return nullptr;
}

Avl* makeUnbalanced(int depth, int version) {
    if (version == 0) {
        return new AvlRec(buildUnbalancedBasic(64, depth, nullptr));
    } else if (version == 1) {
        return new AvlIt(buildUnbalancedBasic(64, depth, nullptr));
    } else if (version == 2) {
        auto avl = new AvlVec();
        buildUnbalancedVector(64, depth, avl->getNodes());
        avl->setSize();
        return avl;
    } else if (version == 3) {
        return new AvlComp(buildUnbalancedCompact(64, depth), depth);
    } else if (version == 4) {
        Node* nil = new Node();
        return new AvlDum(buildUnbalancedBasic(64, depth, nil), nil);
    }
    return nullptr;
}

std::vector<long> getFullDeletes(long n) {
    std::srand(0);
    std::vector<long> inserts;
    std::vector<long> insertsTmp;
    std::vector<long> deletes;
    for (size_t i = 0; i < n; i++) {
        long random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
        insertsTmp.push_back(random_value);
    }
    for (size_t i = 0; i < insertsTmp.size(); i++) {
        long random_value = std::rand() % insertsTmp.size();
        deletes.push_back(insertsTmp[random_value]);
        insertsTmp.erase(insertsTmp.begin() + random_value);
        if (insertsTmp.empty()) break;
    }
    return deletes;
}

std::vector<long> getFullInserts(long n) {
    std::srand(0);
    std::vector<long> inserts;
    for (size_t i = 0; i < n; i++) {
        int random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
    }
    return inserts;
}
