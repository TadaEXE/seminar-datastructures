// This class will implement Red Black trees as described in FDL Nipkow Chapter 8.

#pragma once

enum Color { Red, Black };

template <typename T>
class rbt
{
private:
    Color color;
    T mark;
    rbt<T>* left;
    rbt<T>* right;

    void baliL() {
        if (left == nullptr) return;

        // Naming is according to the code in the book.

        // L Red and left child of L red.
        if (left->color == Red && left->left != nullptr && left->left->color == Red) {
            rbt<T>* new_left = left->left;
            rbt<T>* t3 = left->right;
            rbt<T>* t4 = right;

            T b = left->mark;
            T c = mark;

            mark = b;
            left = new_left;
            left->color = Black;
            right = new rbt<T>(c, Black);
            right->left = t3;
            right->right = t4;
            color = Red;
        }
        // L Red and right child of L red. 
        else if (left->color == Red && left->right != nullptr && left->right->color == Red) {
            rbt<T>* t1 = left->left;
            rbt<T>* t2 = left->right->left;
            rbt<T>* t3 = left->right->right;
            rbt<T>* t4 = right;

            T a = left->mark;
            T b = left->right->mark;
            T c = mark;

            mark = b;
            left = new rbt<T>(a, Black);
            left->left = t1;
            left->right = t2;
            right = new rbt<T>(c, Black);
            right->left = t3;
            right->right = t4;
            color = Red;
        }
        // No conflict.
        else {
            color = Black;
        }
    }
    void baliR() {
        return;
    }

    // This is heavily inspired by the pseudo code of insert in the book.
    void ins(T x) {
        if (color = Black) {
            if (x < mark) {
                // Check if child is a real tree.
                if (left != nullptr) {
                    left->ins(x);
                    // Balance tree after x was inserted into child.
                    baliL();
                } 
                // Left child is Empty.
                else {
                    left = new rbt<T>(x, Red);
                }
            } else if (x == mark) {
                // Only change color. TODO: is this necessary?
                color = Black;
            } else {
                // Check if child is a real tree.
                if (right != nullptr) {
                    right->ins(x);
                    // Balance tree after x was inserted into child.
                    baliR();
                } 
                // Right child is Empty.
                else {
                    right = new rbt<T>(x, Red);
                }
            }
        } else {
            if (x < mark) {
                // Check if child is a real tree.
                if (left != nullptr) {
                    left->ins(x);
                    color = Red;
                } 
                // Left child is Empty.
                else {
                    left = new rbt<T>(x, Red);
                }
            } else if (x == mark) {
                color = Red;
            } else {
                // Check if child is a real tree.
                if (right != nullptr) {
                    right->ins(x);
                    color = Red;
                } 
                // Right child is Empty.
                else {
                    right = new rbt<T>(x, Red);
                }
            }
        }
        return;
    }

public:
    rbt(T mark, Color color = Black) {
        this->mark = mark;
        this->color = color;
        this->left = nullptr;
        this->right = nullptr;
    }

    void insert(T x) {
        ins(x);
        color = Black;
        return;
    }


    void delete_node(T x) {
        return;
    }
    bool lookup(T x) {
        return true;
    }
};


