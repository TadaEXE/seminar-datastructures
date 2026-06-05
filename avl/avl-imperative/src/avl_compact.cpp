#include "avl_compact.h"
#include "vector"
#include "cmath"
#include <iostream>
#include <assert.h>


using namespace avl;

#define MASK_MAX (UINT64_MAX - 3)
#define BALANCE(raw) ((uint64_t) (raw) & 3)
#define SET_BALANCE(raw, bal) (((uint64_t) (raw) & MASK_MAX) | (bal))
#define CLEAN(raw) ((CompactNode*) ((raw) & MASK_MAX))

uint64_t AvlComp::balL1(uint64_t current) {
    //std::cout << HEIGHT(current) << ", " << HEIGHT(current->left) << ", " << HEIGHT(current->right) << ", " << HEIGHT(current->left->left) << ", " << HEIGHT(current->left->right) << "\n";
    CompactNode* nodeC = CLEAN(current);
    CompactNode* nodeA = CLEAN(nodeC->left);
    int balanceA = BALANCE(nodeC->left);
    nodeC->left = nodeA->right;
    //Balance of C: 10->00 00->10
    nodeA->right = SET_BALANCE(current, balanceA ^ 2);
    //Balance of A: 10->00 00->01
    uint64_t result = SET_BALANCE(nodeA, balanceA == 0 ? 1 : 0);
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return result;
}

uint64_t AvlComp::balL2(uint64_t current) {
    //std::cout << "BalL2: " << CLEAN(current)->key << "\n";
    CompactNode* nodeC = CLEAN(current);
    CompactNode* nodeA = CLEAN(nodeC->left);
    CompactNode* nodeB = CLEAN(nodeA->right);
    //10
    int balanceB = BALANCE(nodeA->right);
    nodeA->right = nodeB->left;
    nodeC->left = nodeB->right;
    //Balance of B: 00->00 10->00 01-> 10
    //Should be 00
    nodeB->left = SET_BALANCE(nodeA, balanceB == 1 ? 2 : 0);
    //Should be 01
    nodeB->right = SET_BALANCE(nodeC, balanceB == 2 ? 1 : 0);
    return (uint64_t) nodeB;
}

uint64_t AvlComp::balR1(uint64_t current) {
    CompactNode* nodeC = CLEAN(current);
    CompactNode* nodeA = CLEAN(nodeC->right);
    int balanceA = BALANCE(nodeC->right);
    nodeC->right = nodeA->left;
    //Balance of C: 01->00 00->01
    nodeA->left = SET_BALANCE(current, balanceA ^ 1);
    //Balance of A: 01->00 00->10
    uint64_t result = SET_BALANCE(nodeA, balanceA == 0 ? 2 : 0);
    //std::cout << HEIGHT(left) << ", " << HEIGHT(left->left) << ", " << HEIGHT(left->right) << "\n";
    return result;
}

uint64_t AvlComp::balR2(uint64_t current) {
    CompactNode* nodeC = CLEAN(current);
    CompactNode* nodeA = CLEAN(nodeC->right);
    CompactNode* nodeB = CLEAN(nodeA->left);
    int balanceB = BALANCE(nodeA->left);
    nodeA->left = nodeB->right;
    nodeC->right = nodeB->left;
    //Balance of B: 00->00 10->00 01-> 10
    nodeB->right = SET_BALANCE(nodeA, balanceB == 2 ? 1 : 0);
    nodeB->left = SET_BALANCE(nodeC, balanceB == 1 ? 2 : 0);
    return (uint64_t) nodeB;
}

void AvlComp::ins(int x) {
    std::vector<uint64_t> trace;
    uint64_t current = this->root;
    if (current == 0) {
        this->root = (uint64_t) new CompactNode(x);
        height++;
        return;
    }
    bool isDeepest = true;
    trace.resize(height);
    int traceSize = 0;
    //Map out the path from root to insertion spot of x
    while (current != 0) {
        //std::cout << current->height << ", " << traceSize << "\n";
        trace[traceSize++] = current;
        if (CLEAN(current)->key == x) {
            return;
        } else if (x < CLEAN(current)->key) {
            //Going down into the deepest branch if current is not right heavy
            isDeepest &= !(BALANCE(current) & 1);
            current = CLEAN(current)->left;
        } else {
            //Going down into the deepest branch if current is not right heavy
            isDeepest &= !(BALANCE(current) & 1);
            current = CLEAN(current)->right;
        }
    }
    uint64_t modChild = (uint64_t) new CompactNode(x);
    int index = traceSize; 
    bool hasChanged = true;
    //Assumes height of one child has changed, 
    //returns as soon as height does not change anymore
    while (index-- > 0 && hasChanged) {
        current = trace[index];
        //std::cout << "Processing: " << CLEAN(current)->key << ", " << BALANCE(current) << ", " << BALANCE(modChild) << "\n";
        CompactNode* currentClean = CLEAN(current);
        //std::cout << "Processing: " << currentClean->key << "\n";
        //The left subtree has increased height
        if (CLEAN(modChild)->key < currentClean->key) {
            currentClean->left = modChild;
            //The node is now left heavy and the height increases
            if (BALANCE(current) == 0) {
                modChild = SET_BALANCE(current, 2);
            } else if (BALANCE(current) == 1) {
                //The node is now balanced, height did not change
                modChild = SET_BALANCE(current, 0);
                hasChanged = false;
            } else {
                //The node was already left heavy, so rebalance is neccessary
                if (BALANCE(modChild) == 2) {
                    modChild = balL1(current);
                    hasChanged = false;
                } else if (BALANCE(modChild) == 0) {
                    modChild = balL1(current);
                } else {
                    modChild = balL2(current);
                    hasChanged = false;
                }
                //std::cout << "C1 " << CLEAN(modChild)->key << ", " << ", " << BALANCE(modChild) << "\n";
            }
        }
        else {
            currentClean->right = modChild;
            //The node is now right heavy and the height increases
            if (BALANCE(current) == 0) {
                modChild = SET_BALANCE(current, 1);
            } else if (BALANCE(current) == 2) {
                //The node is now balanced, height did not change
                modChild = SET_BALANCE(current, 0);
                hasChanged = false;
            } else {
                //The node was already right heavy, so rebalance is neccessary
                if (BALANCE(modChild) == 1) {
                    modChild = balR1(current);
                    hasChanged = false;
                } else if (BALANCE(modChild) == 0) {
                    modChild = balR1(current);
                } else {
                    modChild = balR2(current);
                    hasChanged = false;
                }
            }
        }
        //std::cout << "Next: " << modChild << "\n";
    }
    //The height of the root changed
    if (hasChanged) {
        height++;
    }
    if (index == -1) {
        //std::cout << "New root: " << CLEAN(modChild)->key << ", " << CLEAN(modChild)->left << ", " << CLEAN(modChild)->right << "\n";
        root = modChild;
    } else if (CLEAN(modChild)->key < CLEAN(trace[index])->key) {
        CLEAN(trace[index])->left = modChild;
    } else {
        CLEAN(trace[index])->right = modChild;
    }
}

void AvlComp::del(int x) {
    if (this->root == 0) return;
    std::vector<uint64_t> trace;
    uint64_t current = this->root;
    trace.resize(height);
    int traceSize = 0;
    //The node with key x. The key is replaced by the predecesor
    uint64_t replaced = 0;
    //Map out the path from root to the deleted node
    while (current != 0) {
        trace[traceSize++] = current;
        //The current node has key x
        if (replaced != 0) {
            current = CLEAN(current)->right;
        } else if (CLEAN(current)->key == x) {
            replaced = current;
            current = CLEAN(current)->left;
        } else if (x < CLEAN(current)->key) {
            current = CLEAN(current)->left;
        } else {
            current = CLEAN(current)->right;
        }
    }
    //The AVL tree did not contain the key x
    if (replaced == 0) return;
    uint64_t modChild = 0;
    bool isLeftChild = 0;

    if (replaced == trace[traceSize - 1]) {
        modChild = CLEAN(replaced)->right;
        isLeftChild = x < CLEAN(trace[traceSize - 2])->key;
    } else if (replaced == trace[traceSize - 2]) {
        //std::cout << "B: " << traceSize << "," << trace[traceSize - 2]->key << ", "<< trace[traceSize - 1]->key << "\n";
        modChild = CLEAN(trace[traceSize - 1])->left;
        isLeftChild = true;
    } else {
        //TODO merge two cases
        //std::cout << "C: " << traceSize << "\n";
        modChild = CLEAN(trace[traceSize - 1])->left;
        isLeftChild = false;
    }

    delete CLEAN(trace[traceSize - 1]);
    //Skip the last node of the trace since it has been deleted
    int index = traceSize - 1;
    bool hasChanged = true;
    
    while (index-- > 0 && hasChanged)
    {
        current = trace[index];
        CompactNode* currentClean = CLEAN(current);
        //The left subtree of the node was modified
        isLeftChild = modChild != 0 ? CLEAN(modChild)->key < currentClean->key : isLeftChild;
        if (isLeftChild) {
            currentClean->left = modChild;
            //Node was left heavy and is now balanced
            //Height of the node changed
            if (BALANCE(current) == 2) {
                modChild = SET_BALANCE(current, 0);
            } else if (BALANCE(current) == 0) {
                //Node was balanced and is now right heavy
                modChild = SET_BALANCE(current, 1);
                hasChanged = false;
            } else {
                //The node was already right heavy, so rebalance is neccessary
                //Rebalance depends on the heights in the other subtee
                //std::cout << "Z1: " << currentClean->key << "\n";
                uint64_t rightChild = currentClean->right;
                if (BALANCE(rightChild) == 1) {
                    modChild = balR1(current);
                } else if (BALANCE(rightChild) == 0) {
                    modChild = balR1(current);
                    hasChanged = false;
                } else {
                    modChild = balR2(current);
                }
            }
        } else {
            currentClean->right = modChild;
            //Node was right heavy and is now balanced
            //Height of the node changed
            if (BALANCE(current) == 1) {
                modChild = SET_BALANCE(current, 0);
            } else if (BALANCE(current) == 0) {
                //Node was balanced and is now left heavy
                modChild = SET_BALANCE(current, 2);
                hasChanged = false;
            } else {
                //The node was already left heavy, so rebalance is neccessary
                //Rebalance depends on the heights in the other subtee
                uint64_t leftChild = currentClean->left;
                //std::cout << "Z2: " << currentClean->key << "\n";
                if (BALANCE(leftChild) == 2) {
                    modChild = balL1(current);
                } else if (BALANCE(leftChild) == 0) {
                    modChild = balL1(current);
                    hasChanged = false;
                } else {
                    modChild = balL2(current);
                }
            }
        }
    }
    if (hasChanged) {
        height--;
    }

    if (index == -1) {
        root = modChild;
    } else if (CLEAN(modChild)->key < CLEAN(trace[index])->key) {
        CLEAN(trace[index])->left = modChild;
    } else {
        CLEAN(trace[index])->right = modChild;
    }
}

bool AvlComp::find(int x) {
    CompactNode* current = CLEAN(root);
    while (current != nullptr) {
        if (x == current->key) return true;
        else if (x < current->key) current = CLEAN(current->left);
        else current = CLEAN(current->right);
    }
    return false;
}

CompactNode* AvlComp::getRoot() { return CLEAN(root); }

int AvlComp::checkAndHeight(uint64_t current) {
    if (current == 0) return 0;
    CompactNode* currentClean = CLEAN(current);
    int leftHeight = checkAndHeight(currentClean->left);
    int rightHeight = checkAndHeight(currentClean->right);
    //std::cout << "Checking key " << currentClean->key << ": " << leftHeight << ", " << rightHeight << "\n";
    //std::cout << "Children: " << ((currentClean->left != 0) ? CLEAN(currentClean->left)->key : 0) << ", " << ((currentClean->right != 0) ? CLEAN(currentClean->right)->key : 0) << "\n"; 
    if (leftHeight == -1 || rightHeight == -1) return -1;
    //Check that the search tree invariant is valid
    if (leftHeight > 0 && CLEAN(currentClean->left)->key >= currentClean->key) {
        std::cout << "Invalid left key\n";
        return -1;
    } 
    if (rightHeight > 0 && CLEAN(currentClean->right)->key <= currentClean->key) {
        std::cout << "Invalid left key\n";
        return -1;
    }
    if (leftHeight == rightHeight + 1) {
        if (BALANCE(current) != 2) {
            std::cout << "Node " << CLEAN(current)->key << " should be left heavy\n";
            return -1;
        }
    } else if (leftHeight == rightHeight) {
        if (BALANCE(current) != 0) {
            std::cout << "Node " << CLEAN(current)->key << " should be balanced\n";
            return -1;
        }
    } else if (leftHeight + 1 == rightHeight) {
        if (BALANCE(current) != 1) {
            std::cout << "Node " << CLEAN(current)->key << " should be right heavy\n";
            return -1;
        }
    } else {
        std::cout << "Node " << currentClean->key << " violates avl invariant: " << leftHeight << ", " << rightHeight << "\n";
        return -1;
    }
    return leftHeight >= rightHeight ? leftHeight + 1 : rightHeight + 1;
}

void printTreeRec(uint64_t current) {
    if (current == 0) return;
    CompactNode* currentClean = CLEAN(current);
    std::cout << currentClean->key << ": " << (currentClean->left != 0 ? CLEAN(currentClean->left)->key : 0) <<
        ", " << BALANCE(current) << ", " << (currentClean->right != 0 ? CLEAN(currentClean->right)->key : 0) << "\n";
    printTreeRec(currentClean->left);
    printTreeRec(currentClean->right);

}

void AvlComp::printTree() {
    printTreeRec(root);
}

bool AvlComp::checkInv() {
    int heightCalc = checkAndHeight(root);
    if (heightCalc == -1) return false;
    return heightCalc == height;
}