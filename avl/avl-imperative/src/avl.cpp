#include "avl.h"
#include <iostream>
#include <algorithm>

int height(Node* n) {
    if (n == nullptr) return 0;
    return n->height;
}

// Height = 1 + max(left or right subtree heights)
void updateHeight(Node* n) {
    if (n == nullptr) return;
    n->height = 1 + std::max(height(n->left), height(n->right));
}

// left side grew too tall, rebalance by rotating right
// LL: single rotation, LR: double rotation
// balance factor = height(left) - height(right), if > 1 then left is too tall
Node* AvlRec::balL(Node* z) {
    Node* left  = z->left;
    Node* right = z->right;

    // if balance is not violated, just update height and return
    if (height(left) != height(right) + 2) {
        updateHeight(z);
        return z;
    }

    Node* leftLeft  = left->left;
    Node* leftRight = left->right;

    if (height(leftRight) <= height(leftLeft)) {
        // LL-Fall: einfache Rechtsrotation
        left->right = z;
        z->left     = leftRight;
        updateHeight(z);
        updateHeight(left);
        return left;
    } else {
        // LR-Fall: Linksrotation, danach Rechtsrotation
        Node* lrLeft  = leftRight->left;
        Node* lrRight = leftRight->right;

        left->right      = lrLeft;
        z->left          = lrRight;
        leftRight->left  = left;
        leftRight->right = z;

        updateHeight(left);
        updateHeight(z);
        updateHeight(leftRight);
        return leftRight;
    }
}

// right side grew too tall, mirror of balL
// RR: single rotation, RL: double rotation
Node* AvlRec::balR(Node* z) {
    Node* left  = z->left;
    Node* right = z->right;

    if (height(right) != height(left) + 2) {
        updateHeight(z);
        return z;
    }

    Node* rightLeft  = right->left;
    Node* rightRight = right->right;

    if (height(rightLeft) <= height(rightRight)) {
        // RR: sola donus
        right->left = z;
        z->right    = rightLeft;
        updateHeight(z);
        updateHeight(right);
        return right;
    } else {
        // RL: once saga sonra sola donus
        Node* rlLeft  = rightLeft->left;
        Node* rlRight = rightLeft->right;

        z->right         = rlLeft;
        right->left      = rlRight;
        rightLeft->left  = z;
        rightLeft->right = right;

        updateHeight(z);
        updateHeight(right);
        updateHeight(rightLeft);
        return rightLeft;
    }
}

bool AvlRec::find(long key) {
    return search(root, key);
}

// search for a key in the tree, standard binary search tree search
// if found return true, if not found return false
// if key is less than current node, go left, if greater go right
bool AvlRec::search(Node* n, long key) {
    if (n == nullptr) return false;
    if (key == n->key) return true;
    if (key < n->key) return search(n->left, key);
    return search(n->right, key);
}

void AvlRec::ins(long key) {
    root = insert(root, key);
}

// go left -> balL, go right -> balR
Node* AvlRec::insert(Node* n, long key) {
    if (n == nullptr) return new Node(key);

    if (key < n->key) {
        n->left = insert(n->left, key);
        return balL(n);
    } else if (key > n->key) {
        n->right = insert(n->right, key);
        return balR(n);
    }
    return n; // already exists
}

// remove and return the max element, right side shrinks so call balL
// used in deleteNode when we need to find the in-order predecessor
// (max of left subtree)
// if right child is null, then current node is max, return left child as remaining tree
// else go right and keep looking for max, then balL to fix the tree
// returns pair of (remaining tree after removing max, max key)
std::pair<Node*, long> AvlRec::split_max(Node* t) {
    if (t->right == nullptr) {
        long maxKey = t->key;
        Node* remaining = t->left;
        delete t;
        return {remaining, maxKey};
    }
    auto [newRight, maxKey] = split_max(t->right);
    t->right = newRight;
    return {balL(t), maxKey};
}

void AvlRec::del(long key) {
    root = deleteNode(root, key);
}

// opposite of insert: left shrinks -> balR, right shrinks -> balL
Node* AvlRec::deleteNode(Node* n, long key) {
    if (n == nullptr) return nullptr;

    if (key < n->key) {
        n->left = deleteNode(n->left, key);
        return balR(n); // left shrank
    } else if (key > n->key) {
        n->right = deleteNode(n->right, key);
        return balL(n); // right shrank
    }

    // found the node
    if (n->left == nullptr) {
        Node* r = n->right;
        delete n;
        return r;
    }

    // replace with in-order predecessor (max of left subtree)
    auto [newLeft, predKey] = split_max(n->left);
    n->key  = predKey;
    n->left = newLeft;
    return balR(n); // left shrank
}

void AvlRec::free() {
    freeTree(root);
    this->root = nullptr;
}

bool AvlRec::checkInv() {
    return avl_inv(root) && search_tree_inv(root);
}

void inorder(Node* n) {
    if (n == nullptr) return;
    inorder(n->left);
    std::cout << n->key << " ";
    inorder(n->right);
}

void freeTree(Node* n) {
    if (n == nullptr || n->height == 0) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

// check stored height is correct and balance difference is at most 1
bool avl_inv(Node* n) {
    if (n == nullptr) return true;
    if (!avl_inv(n->left) || !avl_inv(n->right)) return false;

    int lh = height(n->left);
    int rh = height(n->right);
    // check that the stored height matches the actual height
    if (n->height != 1 + std::max(lh, rh)) {
        std::cout << "Height: " << n << ", " << n->key << ", " << n->height << ", " << n->left->height << ", " << n->right->height << "\n";
        return false;
    }

    // check balance
    int diff = lh - rh;
    return diff >= -1 && diff <= 1;
}

// check that every key is within the valid range for its position in the tree
bool search_tree_inv(Node* n, long long min, long long max) {
    if (n == nullptr) return true;

    if (n->key < min || n->key > max) return false;
    return search_tree_inv(n->left, min, n->key)
        && search_tree_inv(n->right, n->key, max);
}

long AvlRec::min() {
    Node* t = root;
    while (t->left) t = t->left;
    return t->key;
}

long AvlRec::max() {
    Node* t = root;
    while (t->right) t = t->right;
    return t->key;
}
