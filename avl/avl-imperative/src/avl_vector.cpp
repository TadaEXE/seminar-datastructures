#include "avl_vector.h"
#include "vector"
#include "cmath"
#include <iostream>
#include <assert.h>

#define HEIGHT(index, nodes) (index >= 0 ? nodes[index].height : 0)
#define CALC_HEIGHT(tree) (std::max(tree.left != nullptr ? tree->left->height : 0, tree->right != nullptr ? tree->right->height : 0) + 1) 

using namespace avl;

void AvlVec::addNode(int key) {
    std::size_t old_cap = nodes.capacity();
    nodes.resize(nodes.size() + 1);
    std::size_t new_cap = nodes.capacity();
    if (new_cap > old_cap)
        VectorNode::bytes_allocated += static_cast<long long>((new_cap - old_cap) * sizeof(VectorNode));
    nodes[nodes.size() - 1].height = 1;
    nodes[nodes.size() - 1].key = key;
    nodes[nodes.size() - 1].left = -1;
    nodes[nodes.size() - 1].right = -1;
    size++;
}

int AvlVec::balL1(int current) {
    //std::cout << HEIGHT(current) << ", " << HEIGHT(current->left) << ", " << HEIGHT(current->right) << ", " << HEIGHT(current->left->left) << ", " << HEIGHT(current->left->right) << "\n";
    int left = nodes[current].left;
    int heightB = HEIGHT(nodes[left].right, nodes);
    nodes[current].left = nodes[left].right;
    //if (nodes[left].right > 0) nodes[nodes[left].right].parent = current;
    nodes[left].right = current;
    //nodes[current].parent = left;
    nodes[current].height = heightB + 1;
    nodes[left].height = heightB + 2;
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return left;
}

int AvlVec::balL2(int current) {
    int left = nodes[current].left;
    int treeB = nodes[left].right;
    nodes[left].right = nodes[treeB].left;
    nodes[current].left = nodes[treeB].right;
    nodes[treeB].left = left;
    nodes[treeB].right = current;
    nodes[left].height = nodes[treeB].height;
    nodes[current].height = nodes[treeB].height;
    nodes[treeB].height++;
    return treeB;
}

int AvlVec::balR1(int current) {
    int right = nodes[current].right;
    int heightB = HEIGHT(nodes[right].left, nodes);
    nodes[current].right = nodes[right].left;
    nodes[right].left = current;
    nodes[current].height = heightB + 1;
    nodes[right].height = heightB + 2;
    return right;
}

int AvlVec::balR2(int current) {
    int right = nodes[current].right;
    int treeB = nodes[right].left;
    nodes[right].left = nodes[treeB].right;
    nodes[current].right = nodes[treeB].left;
    nodes[treeB].left = current;
    nodes[treeB].right = right;
    nodes[right].height = nodes[treeB].height;
    nodes[current].height = nodes[treeB].height;
    nodes[treeB].height++;
    //std::cout << HEIGHT(treeB) << ", " << HEIGHT(treeB->left) << ", " << HEIGHT(treeB->right) << "\n";
    return treeB;
}

bool AvlVec::balance(int current, int parent) {
    int leftHeight = HEIGHT(nodes[current].left, nodes);
    int rightHeight = HEIGHT(nodes[current].right, nodes);
    //std::cout << leftHeight << ", " << rightHeight << "\n";
    int res = -1;
    //Save wether the height of the tree changed because of the rebalance
    bool changed;
    if (leftHeight > rightHeight && HEIGHT(nodes[nodes[current].left].left, nodes) >= 
        HEIGHT(nodes[nodes[current].left].right, nodes)) {
        changed = HEIGHT(nodes[nodes[current].left].left, nodes) > 
            HEIGHT(nodes[nodes[current].left].right, nodes);
        res = balL1(current);
    } else if (leftHeight > rightHeight) {
        changed = true;
        res = balL2(current);
    } else if (HEIGHT(nodes[nodes[current].right].right, nodes) >= 
            HEIGHT(nodes[nodes[current].right].left, nodes)) {
        changed = HEIGHT(nodes[nodes[current].right].right, nodes) > 
            HEIGHT(nodes[nodes[current].right].left, nodes);
        res = balR1(current);
    } else {
        changed = true;
        res = balR2(current);
    }
    if (parent == -1) this->root = res;
    else if (nodes[res].key < nodes[parent].key) nodes[parent].left = res; 
    else nodes[parent].right = res; 
    return changed;
}


void AvlVec::ins(int x) {
    std::vector<int> trace;
    int current = this->root;
    if (current == -1) {
        addNode(x);
        this->root = 0;
        return;
    }
    trace.resize(nodes[current].height);
    int traceSize = 0;
    //Map out the path from root to insertion spot of x
    while (current != -1) {
        trace[traceSize++] = current;
        if (nodes[current].key == x) {
            return;
        } else if (x < nodes[current].key) {
            current = nodes[current].left;
        } else {
            current = nodes[current].right;
        }
    }
    int placeToInsert = trace[traceSize - 1];
    addNode(x);
    if (x < nodes[placeToInsert].key) {
        nodes[placeToInsert].left = nodes.size() -1;
    } else {
        nodes[placeToInsert].right = nodes.size() -1;
    }
    //Assumes height of one child has changed, 
    //returns as soon as height does not change anymore
    for (int i = traceSize - 1; i >= 0; i--) {
        current = trace[i];
        int leftHeight = nodes[current].left != -1 ? nodes[nodes[current].left].height : 0;
        int rightHeight = nodes[current].right != -1 ? nodes[nodes[current].right].height : 0;

        if (leftHeight == rightHeight) {
            //The current node has become balanced
            return;
        } else if (abs(leftHeight - rightHeight) == 1) {
            //The current node was balanced and is now imbalanced 
            nodes[current].height++;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : -1);
            //The rebalancing reduced the height to the value before inserting
            if (changed) return;
        }
    }
}

void AvlVec::del(int x) {
    if (this->root == -1) return;
    std::vector<int> trace;
    int current = this->root;
    if (current != -1) trace.resize(nodes[current].height);
    int traceSize = 0;
    //The node with key x. The key is replaced by the predecesor
    int replaced = -1;
    //Map out the path from root to the deleted node
    while (current != -1) {
        trace[traceSize++] = current;
        //The current node has key x
        if (replaced != -1) {
            current = nodes[current].right;
        } else if (nodes[current].key == x) {
            replaced = current;
            current = nodes[current].left;
        } else if (x < nodes[current].key) {
            current = nodes[current].left;
        } else {
            current = nodes[current].right;
        }
    }
    //The AVL tree did not contain the key x
    if (replaced == -1) return;
    //If the deleted node does not have a left subtree, it is replaced by its right subtree 
    if (replaced == trace[traceSize - 1]) {
        //std::cout << "A\n";
        if (traceSize == 1) root = nodes[replaced].right;
        else if (x < nodes[trace[traceSize - 2]].key) nodes[trace[traceSize - 2]].left = nodes[replaced].right;
        else nodes[trace[traceSize - 2]].right = nodes[replaced].right;
    } else if (replaced == trace[traceSize - 2]) {
        //std::cout << "B: " << traceSize << "," << trace[traceSize - 2]->key << ", "<< trace[traceSize - 1]->key << "\n";
        nodes[replaced].key = nodes[trace[traceSize - 1]].key;
        nodes[trace[traceSize - 2]].left = nodes[trace[traceSize - 1]].left;
    } else {
        //std::cout << "C: " << traceSize << "\n";
        nodes[replaced].key = nodes[trace[traceSize - 1]].key;
        nodes[trace[traceSize - 2]].right = nodes[trace[traceSize - 1]].left;
    }
    nodes[trace[traceSize - 1]].height = -1;
    size--;
    
    for (int i = traceSize - 2; i >= 0; i--) {
        current = trace[i];
        if (abs(HEIGHT(nodes[current].left, nodes) - HEIGHT(nodes[current].right, nodes)) == 1) {
            //Height is now unbalanced, and did not change
            break;
        } else if (HEIGHT(nodes[current].left, nodes) == HEIGHT(nodes[current].right, nodes)) {
            //Height changed and node is now balanced
            nodes[current].height --;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : -1); 
            //The height is the same as before the deletion
            if (!changed) break; 
        }
    }
    compress();
}

bool AvlVec::find(int x) {
    int current = root;
    while (current != -1) {
        if (x == nodes[current].key) return true;
        else if (x < nodes[current].key) current = nodes[current].left;
        else current = nodes[current].right;
    }
    return false;
}

void AvlVec::compress() {
    if (size < nodes.size() / 2) {
        if (root == -1) nodes.clear();
        int index = -1;
        //Relocate all nodes that exceed the size after compression 
        for (int i = size; i < nodes.size(); i++) {
            if (nodes[i].height <= 0) continue;
            //Move node to the first free location in the array
            while (nodes[++index].height > 0) { continue; }
            if (root == i) root = index;
            nodes[index] = nodes[i];
            //Save new location of node
            nodes[i].left = index;
        }
        assert(index <= size);
        for (int i = 0; i < size; i++) {
            if (nodes[i].left >= size) {
                nodes[i].left = nodes[nodes[i].left].left;
            } 
            if (nodes[i].right >= size) {
                nodes[i].right = nodes[nodes[i].right].left;
            }
        }
        nodes.resize(size);
    }
}

bool AvlVec::checkInv() {
    if (this->root >= (int) nodes.size()) {
        std::cout << "Invalid root: " << root << ", " << nodes.size() << "\n";
        return false;
    }
    int nodeCount = 0;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].height > 0) {
            nodeCount++;
            if (nodes[i].left >= nodes.size() && nodes[i].left != -1 || nodes[i].right >= nodes.size() && nodes[i].right != -1) {
                std::cout << "Out of bounds: " << i << ", " << nodes[i].left << ", " << nodes[i].right << ", " << nodes.size() << "\n";
                return false;
            }
            if (nodes[i].height != 1 + std::max(HEIGHT(nodes[i].left, nodes), HEIGHT(nodes[i].right, nodes))) {
                std::cout << "Incorrect height " << i << ";" << nodes[i].key << ", " << nodes[i].height << ", " << nodes[i].left << ", " << nodes[i].right << "\n";
                return false;
            }
            if (std::abs(HEIGHT(nodes[i].left, nodes) - HEIGHT(nodes[i].right, nodes)) > 1) {
                std::cout << "Tree unbalanced\n";
                return false;
            }
            if (nodes[i].left != -1 && nodes[nodes[i].left].key >= nodes[i].key || 
                nodes[i].right != -1 && nodes[nodes[i].right].key <= nodes[i].key) {
                std::cout << "Invalid search tree\n";
                return false;
            }
        }
    }
    if (nodeCount != size) {
        std::cout << "Expected size: " << nodeCount << " but was: " << size << "\n"; 
    }
    return true;
}


void AvlVec::compareSizes() {
    std::cout << "Cap: " << nodes.capacity() << ", size: " << size << "\n";
}

void AvlVec::printLimit() {
    std::cout << "Max address: " << nodes.data() + nodes.capacity() << "\n";
}

void AvlVec::free() {
    nodes = std::vector<VectorNode>();
    root = -1;
    size = 0;
}

std::vector<VectorNode>* AvlVec::getNodes() {
    return &nodes;
}

int AvlVec::min() {
    if (root == -1) return 0;
    int t = root;
    while (nodes[t].left != -1) t = nodes[t].left;
    return nodes[t].key;
}

int AvlVec::max() {
    if (root == -1) return 0;
    int t = root;
    while (nodes[t].right != -1) t = nodes[t].right;
    return nodes[t].key;
}