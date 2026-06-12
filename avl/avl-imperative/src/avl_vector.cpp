#include "avl_vector.h"
#include "vector"
#include "cmath"
#include <iostream>
#include <assert.h>

#define HEIGHT(index, nodes) (index != SIZE_MAX ? nodes[index].height : 0)

using namespace avl;

void AvlVec::addNode(long key) {
    size_t old_cap = nodes.capacity();
    nodes.resize(nodes.size() + 1);
    size_t new_cap = nodes.capacity();
    if (new_cap > old_cap)
        VectorNode::bytes_allocated += static_cast<long long>((new_cap - old_cap) * sizeof(VectorNode));
    nodes[nodes.size() - 1].height = 1;
    nodes[nodes.size() - 1].key = key;
    nodes[nodes.size() - 1].left = SIZE_MAX;
    nodes[nodes.size() - 1].right = SIZE_MAX;
    size++;
}

size_t AvlVec::balL1(size_t current) {
    //std::cout << HEIGHT(current) << ", " << HEIGHT(current->left) << ", " << HEIGHT(current->right) << ", " << HEIGHT(current->left->left) << ", " << HEIGHT(current->left->right) << "\n";
    size_t left = nodes[current].left;
    size_t heightB = HEIGHT(nodes[left].right, nodes);
    nodes[current].left = nodes[left].right;
    //if (nodes[left].right > 0) nodes[nodes[left].right].parent = current;
    nodes[left].right = current;
    //nodes[current].parent = left;
    nodes[current].height = heightB + 1;
    nodes[left].height = heightB + 2;
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return left;
}

size_t AvlVec::balL2(size_t current) {
    size_t left = nodes[current].left;
    size_t treeB = nodes[left].right;
    nodes[left].right = nodes[treeB].left;
    nodes[current].left = nodes[treeB].right;
    nodes[treeB].left = left;
    nodes[treeB].right = current;
    nodes[left].height = nodes[treeB].height;
    nodes[current].height = nodes[treeB].height;
    nodes[treeB].height++;
    return treeB;
}

size_t AvlVec::balR1(size_t current) {
    size_t right = nodes[current].right;
    size_t heightB = HEIGHT(nodes[right].left, nodes);
    nodes[current].right = nodes[right].left;
    nodes[right].left = current;
    nodes[current].height = heightB + 1;
    nodes[right].height = heightB + 2;
    return right;
}

size_t AvlVec::balR2(size_t current) {
    size_t right = nodes[current].right;
    size_t treeB = nodes[right].left;
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

bool AvlVec::balance(size_t current, size_t parent) {
    int leftHeight = HEIGHT(nodes[current].left, nodes);
    int rightHeight = HEIGHT(nodes[current].right, nodes);
    //std::cout << leftHeight << ", " << rightHeight << "\n";
    size_t res = SIZE_MAX;
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
    if (parent == SIZE_MAX) this->root = res;
    else if (nodes[res].key < nodes[parent].key) nodes[parent].left = res; 
    else nodes[parent].right = res; 
    return changed;
}


void AvlVec::ins(long x) {
    std::vector<size_t> trace;
    size_t current = this->root;
    if (current == SIZE_MAX) {
        addNode(x);
        this->root = 0;
        return;
    }
    trace.resize(nodes[current].height);
    int traceSize = 0;
    //Map out the path from root to insertion spot of x
    while (current != SIZE_MAX) {
        trace[traceSize++] = current;
        if (nodes[current].key == x) {
            return;
        } else if (x < nodes[current].key) {
            current = nodes[current].left;
        } else {
            current = nodes[current].right;
        }
    }
    size_t placeToInsert = trace[traceSize - 1];
    addNode(x);
    if (x < nodes[placeToInsert].key) {
        nodes[placeToInsert].left = nodes.size() - 1;
    } else {
        nodes[placeToInsert].right = nodes.size() - 1;
    }
    //Assumes height of one child has changed, 
    //returns as soon as height does not change anymore
    for (int i = traceSize - 1; i >= 0; i--) {
        current = trace[i];
        int leftHeight = nodes[current].left != SIZE_MAX ? nodes[nodes[current].left].height : 0;
        int rightHeight = nodes[current].right != SIZE_MAX ? nodes[nodes[current].right].height : 0;

        if (leftHeight == rightHeight) {
            //The current node has become balanced
            return;
        } else if (abs(leftHeight - rightHeight) == 1) {
            //The current node was balanced and is now imbalanced 
            nodes[current].height++;
        } else {
            bool changed = balance(current, i > 0 ? trace[i - 1] : SIZE_MAX);
            //The rebalancing reduced the height to the value before inserting
            if (changed) return;
        }
    }
}

void AvlVec::del(long x) {
    if (this->root == SIZE_MAX) return;
    std::vector<size_t> trace;
    size_t current = this->root;
    if (current != SIZE_MAX) trace.resize(nodes[current].height);
    int traceSize = 0;
    //The node with key x. The key is replaced by the predecesor
    size_t replaced = SIZE_MAX;
    //Map out the path from root to the deleted node
    while (current != SIZE_MAX) {
        trace[traceSize++] = current;
        //The current node has key x
        if (replaced != SIZE_MAX) {
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
    if (replaced == SIZE_MAX) return;
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
            bool changed = balance(current, i > 0 ? trace[i - 1] : SIZE_MAX); 
            //The height is the same as before the deletion
            if (!changed) break; 
        }
    }
    compress();
}

bool AvlVec::find(long x) {
    size_t current = root;
    while (current != SIZE_MAX) {
        if (x == nodes[current].key) return true;
        else if (x < nodes[current].key) current = nodes[current].left;
        else current = nodes[current].right;
    }
    return false;
}

void AvlVec::compress() {
    if (size < nodes.size() / 2) {
        if (root == SIZE_MAX) nodes.clear();
        //index has to be unsigned, so used max value and force overflow
        size_t index = SIZE_MAX;
        //Relocate all nodes that exceed the size after compression 
        for (size_t i = size; i < nodes.size(); i++) {
            if (nodes[i].height <= 0) continue;
            //Move node to the first free location in the array
            while (nodes[++index].height > 0) { continue; }
            if (root == i) root = index;
            nodes[index] = nodes[i];
            //Save new location of node
            nodes[i].left = index;
        }
        //assert(index <= size && index != SIZE_MAX);
        for (size_t i = 0; i < size; i++) {
            if (nodes[i].left >= size && nodes[i].left != SIZE_MAX) {
                nodes[i].left = nodes[nodes[i].left].left;
            } 
            if (nodes[i].right >= size&& nodes[i].right != SIZE_MAX) {
                nodes[i].right = nodes[nodes[i].right].left;
            }
        }
        nodes.resize(size);
        VectorNode::bytes_allocated += nodes.capacity() * sizeof(VectorNode);
    }
}

bool AvlVec::checkInv() {
    if (this->root >= nodes.size() && this->root != SIZE_MAX) {
        std::cout << "Invalid root: " << root << ", " << nodes.size() << "\n";
        return false;
    }
    size_t nodeCount = 0;
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].height > 0) {
            nodeCount++;
            if (nodes[i].left >= nodes.size() && nodes[i].left != SIZE_MAX || nodes[i].right >= nodes.size() && nodes[i].right != SIZE_MAX) {
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
            if (nodes[i].left != SIZE_MAX && nodes[nodes[i].left].key >= nodes[i].key || 
                nodes[i].right != SIZE_MAX && nodes[nodes[i].right].key <= nodes[i].key) {
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
    root = SIZE_MAX;
    size = 0;
}

std::vector<VectorNode>* AvlVec::getNodes() {
    return &nodes;
}

long AvlVec::min() {
    if (root == SIZE_MAX) return 0;
    size_t t = root;
    while (nodes[t].left != SIZE_MAX) t = nodes[t].left;
    return nodes[t].key;
}

long AvlVec::max() {
    if (root == SIZE_MAX) return 0;
    size_t t = root;
    while (nodes[t].right != SIZE_MAX) t = nodes[t].right;
    return nodes[t].key;
}