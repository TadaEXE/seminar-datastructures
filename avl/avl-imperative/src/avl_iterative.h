#include "avl.h"
#include <vector>

namespace avl {

class AvlIt: public Avl {
    public:
        AvlIt() { root = nullptr; };
        AvlIt(Node* root) { this->root = root; };
        void ins(int x);
        void del(int x);
        bool find(int x);
        bool checkInv();
        Node* balL1(Node* current);
        Node* balL2(Node* current);
        Node* balR1(Node* current);
        Node* balR2(Node* current);
        Node* getRoot();

    private:
        Node* root;
        bool balance(Node* current, Node* parent);
};

}
