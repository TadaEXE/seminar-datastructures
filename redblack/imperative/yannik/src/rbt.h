// This class will implement Red Black trees as described in FDL Nipkow Chapter 8.

#include <iostream>
#include <string>

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
            return;
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
            return;
        }
        // No conflict.
        else {
            color = Black;
            return;
        }
    }
    void baliR() {
        if (right == nullptr) return;

        // Naming is according to the code in the book.

        // R is Red and right child of R is Red.
        if (right->color == Red && right->right != nullptr && right->right->color == Red) {
            rbt<T>* t1 = left;
            rbt<T>* t2 = right->left;
            rbt<T>* new_right = right->right;

            T a = mark;
            T b = right->mark;

            mark = b;
            left = new rbt<T>(a, Black);
            left->left = t1;
            left->right = t2;
            right = new_right;
            right->color = Black;
            color = Red;
            return;
        }
        // R is Red and left child of R is Red.
        else if (right->color == Red && right->left != nullptr && right->left->color == Red)  {
            rbt<T>* t1 = left;
            rbt<T>* t2 = right->left->left;
            rbt<T>* t3 = right->left->right;
            rbt<T>* t4 = right->right;

            T a = mark;
            T b = right->left->mark;
            T c = right->mark;

            mark = b;
            left = new rbt<T>(a, Black);
            left->left = t1;
            left->right = t2;
            right = new rbt<T>(c, Black);
            right->left = t3;
            right->right = t4;
            color = Red;
            return;
        }
        // No conflict.
        else {
            color = Black;
            return;
        }
    }

    // This is heavily inspired by the pseudo code of insert in the book.
    void ins(T x) {
        if (color == Black) {
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
                // Only change color.
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

    void baldL() {
        if (left == nullptr) return;

        // Naming is according to the code in the book.

        // L Red.
        if (left->color == Red) {
            left->color = Black;
            color = Red;
            return;
        }
        // R Black. 
        else if (right != nullptr && right->color == Black) {
            right->color = Red;
            baliR();
            return;
        }
        // R Red with left child Black.
         else if (right != nullptr && right->color == Red && right->left != nullptr && right->left->color == Black) {
            rbt<T>* t1 = left;
            rbt<T>* t2 = right->left->left;
            rbt<T>* t3 = right->left->right;
            rbt<T>* t4 = right->right;

            T a = mark;
            T b = right->left->mark;
            T c = right->mark;
            
            mark = b;
            left = new rbt<T>(a, Black);
            left->left = t1;
            left->right = t2;
            right = new rbt<T>(c, Black);
            right->left = t3;
            right->right = t4;
            right->right->color = Red;
            right->baliR();
            color = Red;
            return;
        }
        // No conflict.
        else {
            color = Red;
            return;
        }
    }

    void baldR() {
        if (right == nullptr) return;

        // Naming is according to the code in the book.

        // R Red.
        if (right->color == Red) {
            right->color = Black;
            color = Red;
            return;
        }
        // L Black. 
        else if (left != nullptr && left->color == Black) {
            left->color = Red;
            baliL();
            return;
        }
        // L Red with right child Black.
         else if (left != nullptr && left->color == Red && left->right != nullptr && left->right->color == Black) {
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
            left->left->color = Red;
            left->right = t2;
            left->baliL();
            right = new rbt<T>(c, Black);
            right->left = t3;
            right->right = t4;
            color = Red;
            return;
        }
        // No conflict.
        else {
            color = Red;
            return;
        }
    }

    // Deletion by joining with recursive approach.
    static rbt<T>* del(rbt<T>* tree, T x) {
        if (x < tree->mark) {
            if (tree->left == nullptr) return nullptr;
            if (tree->left->color == Black) {
                tree->left = del(tree->left, x);
                tree->baldL();
            } else {
                tree->left = del(tree->left, x);
                tree->color = Red;
            }
        } else if (x == tree->mark) {
            rbt<T>* joined = join(tree->left, tree->right);
            return joined;
        } else {
            if (tree->right == nullptr) return nullptr;
            if (tree->right->color == Black) {
                tree->right = del(tree->right, x);
                tree->baldR();
            } else {
                tree->right = del(tree->right, x);
                tree->color = Red;
            }
        }
        return tree;
    }

    static rbt<T>* join(rbt<T>* lhs, rbt<T>* rhs) {
        if (lhs == nullptr) return rhs;
        if (rhs == nullptr) return lhs;

        // lhs Red, rhs Red.
        if (lhs->color == Red && rhs->color == Red) {
            rbt<T>* t1 = lhs->left;
            rbt<T>* t4 = rhs->right;

            T a = lhs->mark;
            T c = rhs->mark;

            rbt<T>* t23 = join(lhs->right, rhs->left);
            if (t23 != nullptr && t23->color == Red) {
                rbt<T>* u2 = t23->left;
                rbt<T>* u3 = t23->right;
                T b = t23->mark;

                rbt<T>* joined = new rbt<T>(b, Red);
                joined->left = new rbt<T>(a, Red);
                joined->left->left = t1;
                joined->left->right = u2;
                joined->right = new rbt<T>(c, Red);
                joined->right->left = u3;
                joined->right->right = t4;
                return joined;
            } else {
                rbt<T>* joined = new rbt<T>(a, Red);
                joined->left = t1;
                joined->right = new rbt<T>(c, Red);
                joined->right->left = t23;
                joined->right->right = t4;
                return joined;
            }
        }
        // lhs Black, rhs Black.
        if (lhs->color == Black && rhs->color == Black) {
            rbt<T>* t1 = lhs->left;
            rbt<T>* t4 = rhs->right;

            T a = lhs->mark;
            T c = rhs->mark;

            rbt<T>* t23 = join(lhs->right, rhs->left);
            if (t23 != nullptr && t23->color == Red) {
                rbt<T>* u2 = t23->left;
                rbt<T>* u3 = t23->right;
                T b = t23->mark;

                rbt<T>* joined = new rbt<T>(b, Red);
                joined->left = new rbt<T>(a, Black);
                joined->left->left = t1;
                joined->left->right = u2;
                joined->right = new rbt<T>(c, Black);
                joined->right->left = u3;
                joined->right->right = t4;
                return joined;
            } else {
                rbt<T>* joined = new rbt<T>(a, Black);
                joined->left = t1;
                joined->right = new rbt<T>(c, Black);
                joined->right->left = t23;
                joined->right->right = t4;
                joined->baldL();
                return joined;
            }
        }
        // rhs Red.
        if (rhs->color == Red) {
            rbt<T>* t2 = rhs->left;
            rbt<T>* t3 = rhs->right;

            T a = rhs->mark;

            rbt<T>* joined = new rbt<T>(a, Red);
            joined->left = join(lhs, t2);
            joined->right = t3;
            return joined;
        }
        // lhs Red.
        if (lhs->color == Red) {
            rbt<T>* t1 = lhs->left;
            rbt<T>* t2 = lhs->right;

            T a = lhs->mark;

            rbt<T>* joined = new rbt<T>(a, Red);
            joined->left = t1;
            joined->right = join(t2, rhs);
            return joined;
        }

        // Every case should be covered -> only for compiler.
        return lhs;
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
        rbt<T>* result = del(this, x);
        if (result != nullptr) {
            mark = result->mark;
            left = result->left;
            right = result->right;
            color = Black;
        }
        return;
    }

    bool lookup(T x) {
        if (x < mark) {
            if (left == nullptr) return false;
            return left->lookup(x);
        } else if (x == mark) {
            return true;
        } else {
            if (right == nullptr) return false;
            return right->lookup(x);
        }
    }

    // Claude Code:
    void print(std::string indent = "") {
        // Print right subtree first (top of output)
        if (right != nullptr) {
            right->print(indent + "      ");
        }
        // Print current node
        std::cout << indent 
                << mark 
                << "(" << (color == Red ? "R" : "B") << ")" 
                << std::endl;
        // Print left subtree (bottom of output)
        if (left != nullptr) {
            left->print(indent + "      ");
        }
    }
    // End of Claude Code
};


