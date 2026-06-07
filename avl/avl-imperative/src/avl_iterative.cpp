#include "avl_iterative.h"
#include "vector"
#include "cmath"
#include <iostream>
#include <assert.h>

#define HEIGHT(tree) (tree != nullptr ? tree->height : 0)
#define CALC_HEIGHT(tree) (std::max(tree->left != nullptr ? tree->left->height : 0, tree->right != nullptr ? tree->right->height : 0) + 1) 

using namespace avl;

Node* AvlIt::balL1(Node* current) {
    //std::cout << HEIGHT(current) << ", " << HEIGHT(current->left) << ", " << HEIGHT(current->right) << ", " << HEIGHT(current->left->left) << ", " << HEIGHT(current->left->right) << "\n";
    Node* left = current->left;
    int heightB = HEIGHT(left->right);
    current->left = left->right;
    left->right = current;
    current->height = heightB + 1;
    left->height = heightB + 2;
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return left;
}

Node* AvlIt::balL2(Node* current) {
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

Node* AvlIt::balR1(Node* current) {
    Node* right = current->right;
    int heightB = HEIGHT(right->left);
    current->right = right->left;
    right->left = current;
    current->height = heightB + 1;
    right->height = heightB + 2;
    return right;
}

Node* AvlIt::balR2(Node* current) {
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

bool AvlIt::balance(Node* current, Node* parent) {
    int leftHeight = HEIGHT(current->left);
    int rightHeight = HEIGHT(current->right);
    //std::cout << leftHeight << ", " << rightHeight << "\n";
    Node* res = nullptr;
    //Save wether the height of the tree changed because of the rebalance
    bool changed;
    if (leftHeight > rightHeight && HEIGHT(current->left->left) >= HEIGHT(current->left->right)) {
        //if (doDebug) std::cout << "A1\n";
        changed = HEIGHT(current->left->left) > HEIGHT(current->left->right);
        res = balL1(current);
    } else if (leftHeight > rightHeight) {
        //if (doDebug) std::cout << "B1\n";
        changed = true;
        res = balL2(current);
    } else if (HEIGHT(current->right->right) >= HEIGHT(current->right->left)) {
        //if (doDebug) std::cout << "C1\n";
        changed = HEIGHT(current->right->right) > HEIGHT(current->right->left);
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

/*
bool AvlVec::balance(Node* current, Node* parent) {
    int leftHeight = HEIGHT(current->left);
    int rightHeight = HEIGHT(current->right);
    //std::cout << leftHeight << ", " << rightHeight << "\n";
    Node* res = nullptr;
    //Save wether the height of the tree changed because of the rebalance
    bool changed;
    if (leftHeight > rightHeight && HEIGHT(current->left->left) >= HEIGHT(current->left->right)) {
        changed = HEIGHT(current->left->left) > HEIGHT(current->left->right);
        res = balL1(current);
    } else if (leftHeight > rightHeight) {
        changed = true;
        res = balL2(current);
    } else if (HEIGHT(current->right->right) >= HEIGHT(current->right->left)) {
        changed = HEIGHT(current->right->right) > HEIGHT(current->right->left);
        res = balR1(current);
    } else {
        //std::cout << "D1\n";
        changed = true;
        res = balR2(current);
    }
    if (parent == nullptr) this->root = res;
    else if (res->key < parent->key) parent->left = res; 
    else parent->right = res; 
    return changed;
}
    */

void AvlIt::ins(int x) {
    std::vector<Node*> trace;
    Node* current = this->root;
    if (current == nullptr) {
        this->root = new Node(x);
        return;
    }
    trace.resize(current->height);
    int traceSize = 0;
    //Map out the path from root to insertion spot of x
    while (current != nullptr) {
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
        placeToInsert->left = new Node(x);
    } else {
        placeToInsert->right = new Node(x);
    }
    //Assumes height of one child has changed, 
    //returns as soon as height does not change anymore
    for (int i = traceSize - 1; i >= 0; i--) {
        current = trace[i];
        int leftHeight = current->left != nullptr ? current->left->height : 0;
        int rightHeight = current->right != nullptr ? current->right->height : 0;

        if (leftHeight == rightHeight) {
            //The current node has become balanced
            return;
        } else if (abs(leftHeight - rightHeight) == 1) {
            //The current node was balanced and is now imbalanced 
            current->height++;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : nullptr);
            //The rebalancing reduced the height to the value before inserting
            if (changed) return;
        }
    }
}

void AvlIt::del(int x) {
    if (this->root == nullptr) return;
    std::vector<Node*> trace;
    Node* current = this->root;
    if (current != nullptr) trace.resize(current->height);
    int traceSize = 0;
    //The node with key x. The key is replaced by the predecesor
    Node* replaced = nullptr;
    //Map out the path from root to the deleted node
    while (current != nullptr) {
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
        if (abs(HEIGHT(current->left) - HEIGHT(current->right)) == 1) {
            //Height is now unbalanced, and did not change
            return;
        } else if (HEIGHT(current->left) == HEIGHT(current->right)) {
            //Height changed and node is now balanced
            current->height --;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : nullptr); 
            //The height is the same as before the deletion
            if (!changed) return; 
        }
    }
}

bool AvlIt::find(int x) {
    Node* current = root;
    while (current != nullptr) {
        if (x == current->key) return true;
        else if (x < current->key) current = current->left;
        else current = current->right;
    }
    return false;
}

bool AvlIt::checkInv() {
    return avl_inv(root);
}

Node* AvlIt::getRoot() {
    return root;
}

void AvlIt::free() { 
    freeTree(root);
    this->root = nullptr; 
}

int AvlIt::min() {
    Node* t = root;
    while (t->left) t = t->left;
    return t->key;
}

int AvlIt::max() {
    Node* t = root;
    while (t->right) t = t->right;
    return t->key;
}