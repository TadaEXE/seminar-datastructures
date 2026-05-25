#include "avl.h"
#include <iostream>
#include <algorithm>

int height(Node* n) {
    if (n == nullptr) return 0;
    return n->height;
}

void updateHeight(Node* n) {
    n->height = 1 + std::max(height(n->left), height(n->right));
}

// balL: called when the left child may have grown too tall (Chapter 9, Figure 9.1)
// two cases: LL (single rotation) and LR (double rotation)
Node* balL(Node* z) {
    Node* AB = z->left;
    Node* C  = z->right;

    if (height(AB) != height(C) + 2) {
        updateHeight(z);
        return z;
    }

    Node* A = AB->left;
    Node* B = AB->right;

    if (height(B) <= height(A)) {
        // LL case - single right rotation
        AB->right = z;
        z->left   = B;
        updateHeight(z);
        updateHeight(AB);
        return AB;
    } else {
        // LR case - double rotation
        // B cant be null here since height(B) > height(A) >= 0
        Node* B1 = B->left;
        Node* B2 = B->right;

        AB->right = B1;
        z->left   = B2;
        B->left   = AB;
        B->right  = z;

        updateHeight(AB);
        updateHeight(z);
        updateHeight(B);
        return B;
    }
}

// balR: mirror of balL, called when right child may be too tall (Figure 9.2)
Node* balR(Node* z) {
    Node* A  = z->left;
    Node* BC = z->right;

    if (height(BC) != height(A) + 2) {
        updateHeight(z);
        return z;
    }

    Node* B = BC->left;
    Node* C = BC->right;

    if (height(B) <= height(C)) {
        // RR case - single left rotation
        BC->left = z;
        z->right = B;
        updateHeight(z);
        updateHeight(BC);
        return BC;
    } else {
        // RL case - double rotation
        Node* B1 = B->left;
        Node* B2 = B->right;

        z->right = B1;
        BC->left = B2;
        B->left  = z;
        B->right = BC;

        updateHeight(z);
        updateHeight(BC);
        updateHeight(B);
        return B;
    }
}

// standard BST lookup
bool search(Node* n, int key) {
    if (n == nullptr) return false;
    if (key == n->key) return true;
    if (key < n->key) return search(n->left, key);
    return search(n->right, key);
}

// insert: after going left call balL, after going right call balR (Section 9.2.1)
Node* insert(Node* n, int key) {
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

// split_max: removes and returns the largest element from subtree t (Figure 9.3)
// after removing from the right side, left might be relatively taller so we call balL
std::pair<Node*, int> split_max(Node* t) {
    if (t->right == nullptr) {
        int maxKey = t->key;
        Node* remaining = t->left;
        delete t;
        return {remaining, maxKey};
    }
    auto [newRight, maxKey] = split_max(t->right);
    t->right = newRight;
    return {balL(t), maxKey};
}

// delete: after going left call balR (left shrank), after going right call balL (right shrank)
// key insight from Section 9.2.2: balL and balR are reused here, same as insert
Node* deleteNode(Node* n, int key) {
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

void inorder(Node* n) {
    if (n == nullptr) return;
    inorder(n->left);
    std::cout << n->key << " ";
    inorder(n->right);
}

void freeTree(Node* n) {
    if (n == nullptr) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

// checks that:
// 1. the stored height is actually correct
// 2. the height difference between left and right is at most 1
// same as avl_inv in the ocaml version
bool avl_inv(Node* n) {
    if (n == nullptr) return true;
    if (!avl_inv(n->left) || !avl_inv(n->right)) return false;

    int lh = height(n->left);
    int rh = height(n->right);

    // check that the stored height matches the actual height
    if (n->height != 1 + std::max(lh, rh)) return false;

    // check balance
    int diff = lh - rh;
    return diff >= -1 && diff <= 1;
}

// checks that for every node, left child < node < right child
// same as search_tree_inv in the ocaml version
bool search_tree_inv(Node* n, long long min, long long max) {
    if (n == nullptr) return true;
    if (n->key <= min || n->key >= max) return false;
    return search_tree_inv(n->left, min, n->key)
        && search_tree_inv(n->right, n->key, max);
}
