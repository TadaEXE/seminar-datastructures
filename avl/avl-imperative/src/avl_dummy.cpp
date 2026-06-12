#include "avl_dummy.h"
#include "vector"
#include "cmath"
#include <iostream>
#include <assert.h>


using namespace avl;

Node* AvlDum::balL1(Node* current) {
    //std::cout << HEIGHT(current) << ", " << HEIGHT(current->left) << ", " << HEIGHT(current->right) << ", " << HEIGHT(current->left->left) << ", " << HEIGHT(current->left->right) << "\n";
    Node* left = current->left;
    int heightB = left->right->height;
    current->left = left->right;
    left->right = current;
    current->height = heightB + 1;
    left->height = heightB + 2;
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return left;
}

Node* AvlDum::balL2(Node* current) {
    Node* left = current->left;
    Node* treeB = current->left->right;
    left->right = treeB->left;
    current->left = treeB->right;
    treeB->left = left;
    treeB->right = current;
    left->height = treeB->height;
    current->height = treeB->height;
    treeB->height++;
    return treeB;
}

Node* AvlDum::balR1(Node* current) {
    Node* right = current->right;
    int heightB = right->left->height;
    current->right = right->left;
    right->left = current;
    current->height = heightB + 1;
    right->height = heightB + 2;
    return right;
}

Node* AvlDum::balR2(Node* current) {
    Node* right = current->right;
    Node* treeB = current->right->left;
    right->left = treeB->right;
    current->right = treeB->left;
    treeB->left = current;
    treeB->right = right;
    right->height = treeB->height;
    current->height = treeB->height;
    treeB->height++;
    //std::cout << HEIGHT(treeB) << ", " << HEIGHT(treeB->left) << ", " << HEIGHT(treeB->right) << "\n";
    return treeB;
}

bool AvlDum::balance(Node* current, Node* parent) {
    int leftHeight = current->left->height;
    int rightHeight = current->right->height;
    //std::cout << leftHeight << ", " << rightHeight << "\n";
    Node* res = nullptr;
    //Save wether the height of the tree changed because of the rebalance
    bool changed;
    if (leftHeight > rightHeight && current->left->left->height >= current->left->right->height) {
        //if (doDebug) std::cout << "A1\n";
        changed = current->left->left->height > current->left->right->height;
        res = balL1(current);
    } else if (leftHeight > rightHeight) {
        //if (doDebug) std::cout << "B1\n";
        changed = true;
        res = balL2(current);
    } else if (current->right->right->height >= current->right->left->height) {
        //if (doDebug) std::cout << "C1\n";
        changed = current->right->right->height > current->right->left->height;
        res = balR1(current);
    } else {
        //if (doDebug) std::cout << "D1\n";
        changed = true;
        res = balR2(current);
    }
    if (parent == nullptr) this->root = res;
    else if (res->key < parent->key) parent->left = res; 
    else parent->right = res; 
    return changed;
}

void AvlDum::ins(long x) {
    std::vector<Node*> trace;
    Node* current = this->root;
    if (current == nil) {
        this->root = new Node(x, nil);
        return;
    }
    trace.resize(current->height);
    int traceSize = 0;
    //Map out the path from root to insertion spot of x
    while (current != nil) {
        //std::cout << current->height << ", " << traceSize << "\n";
        trace[traceSize++] = current;
        if (current->key == x) {
            return;
        } else if (x < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    Node* placeToInsert = trace[traceSize - 1];
    if (x < placeToInsert->key) {
        placeToInsert->left = new Node(x, nil);
    } else {
        placeToInsert->right = new Node(x, nil);
    }
    //Assumes height of one child has changed, 
    //returns as soon as height does not change anymore
    for (int i = traceSize - 1; i >= 0; i--) {
        current = trace[i];

        if (current->left->height == current->right->height) {
            //The current node has become balanced
            return;
        } else if (abs(current->left->height - current->right->height) == 1) {
            //The current node was balanced and is now imbalanced 
            current->height++;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : nullptr);
            //The rebalancing reduced the height to the value before inserting
            if (changed) return;
        }
    }
}

void AvlDum::del(long x) {
    if (this->root == nil) return;
    std::vector<Node*> trace;
    Node* current = this->root;
    if (current != nil) trace.resize(current->height);
    int traceSize = 0;
    //The node with key x. The key is replaced by the predecesor
    Node* replaced = nullptr;
    //Map out the path from root to the deleted node
    while (current != nil) {
        trace[traceSize++] = current;
        //The current node has key x
        if (replaced != nullptr) {
            current = current->right;
        } else if (current->key == x) {
            replaced = current;
            current = current->left;
        } else if (x < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    //The AVL tree did not contain the key x
    if (replaced == nullptr) return;
    //If the deleted node does not have a left subtree, it is replaced by its right subtree 
    if (replaced == trace[traceSize - 1]) {
        //std::cout << "A\n";
        if (traceSize == 1) root = replaced->right;
        else if (x < trace[traceSize - 2]->key) trace[traceSize - 2]->left = replaced->right;
        else trace[traceSize - 2]->right = replaced->right;
    } else if (replaced == trace[traceSize - 2]) {
        //std::cout << "B: " << traceSize << "," << trace[traceSize - 2]->key << ", "<< trace[traceSize - 1]->key << "\n";
        replaced->key = trace[traceSize - 1]->key;
        trace[traceSize - 2]->left = trace[traceSize - 1]->left;
    } else {
        //std::cout << "C: " << traceSize << "\n";
        replaced->key = trace[traceSize - 1]->key;
        trace[traceSize - 2]->right = trace[traceSize - 1]->left;
    }
    delete trace[traceSize - 1];
    
    for (int i = traceSize - 2; i >= 0; i--) {
        current = trace[i];
        if (abs(current->left->height - current->right->height) == 1) {
            //Height is now unbalanced, and did not change
            return;
        } else if (current->left->height == current->right->height) {
            //Height changed and node is now balanced
            current->height --;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : nullptr); 
            //The height is the same as before the deletion
            if (!changed) return; 
        }
    }
}

bool AvlDum::find(long x) {
    Node* current = root;
    while (current != nil) {
        if (x == current->key) return true;
        else if (x < current->key) current = current->left;
        else current = current->right;
    }
    return false;
}

bool AvlDum::checkInv() {
    return avl_inv(root);
}

Node* AvlDum::getRoot() {
    return root;
}

void AvlDum::free() { 
    freeTree(root);
    this->root = nil; 
}

long AvlDum::min() {
    Node* t = root;
    while (t->left != nil) t = t->left;
    return t->key;
}

long AvlDum::max() {
    Node* t = root;
    while (t->right != nil) t = t->right;
    return t->key;
}