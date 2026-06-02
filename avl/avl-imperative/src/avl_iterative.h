#include "avl.h"
#include <vector>

namespace avl {

class AvlRec {
    public:
        AvlRec() { root = nullptr; };
        AvlRec(Node* root) { this->root = root; };
        void ins(int x);
        void del(int x);
        bool find(int x);
        Node* getRoot();
        Node* balL1(Node* current);
        Node* balL2(Node* current);
        Node* balR1(Node* current);
        Node* balR2(Node* current);

    private:
        Node* root;
        bool balance(Node* current, Node* parent);
};

}
