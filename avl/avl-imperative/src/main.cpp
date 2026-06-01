#include "avl_iterative.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>		

using namespace avl;

int main() {
    std::srand(0); 
    AvlRec avl;
    std::vector<int> trace;
    for (int i = 0; i < 10000; i++) {
        int random_value = std::rand() % 20000;
        trace.push_back(random_value);
        avl.ins(random_value);

        assert(avl_inv(avl.getRoot()));
        assert(search_tree_inv(avl.getRoot()));
    }
    std::cout << "Start deletion\n";
    for (int i = 0; i < 10000; i++) {
        int random_value = std::rand() % trace.size();
        avl.del(trace[random_value]);
        trace.erase(trace.begin() + random_value);

        assert(avl_inv(avl.getRoot()));
        assert(search_tree_inv(avl.getRoot()));
    }
}

int someMain() {
    //Node* root = new Node(10); // start with an empty tree
    AvlRec avl;
    // --- insert tests ---

    // insert some values
    avl.ins(10);
    avl.ins(20);
    avl.ins(30); // this should need a rotation
    
    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    avl.ins(40);
    avl.ins(50);
    avl.ins(25); // more rotations can happen here

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    avl.ins(5);
    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));
    avl.ins(3);
    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));
    avl.ins(7);

    // inserting an existing value should not change the tree
    avl.ins(25);

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    std::cout << "Tree after insertions: ";
    inorder(avl.getRoot());
    std::cout << "\n";


    /*
    // search test for existing and non-existing values
    assert(search(root, 25));
    assert(search(root, 3));
    assert(!search(root, 99));
    assert(!search(root, 0));

    std::cout << "Search tests passed\n";

    */
    // delete tests
    // delete some values
    avl.del(3);
    avl.del(5);

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    avl.del(20);
    avl.del(30);

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    // deleting a value which is not in the tree should not break anything
    avl.del(99);

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    std::cout << "Tree after some deletions: ";
    inorder(avl.getRoot());
    std::cout << "\n";

    // delete the remaining values
    avl.del(7);
    avl.del(10);
    avl.del(25);
    avl.del(40);
    avl.del(50);

    assert(avl_inv(avl.getRoot()));
    assert(search_tree_inv(avl.getRoot()));

    // now the tree should be empty
    assert(avl.getRoot() == nullptr);

    std::cout << "Tree is empty now\n";
    std::cout << "All tests passed\n";

    freeTree(avl.getRoot());
    return 0;
}