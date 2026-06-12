#include "avl.h"
#include <vector>

namespace avl {

class AvlDum: public Avl {
    public:
        AvlDum() { 
            nil = new Node();
            root = nil; 
        };
        AvlDum(Node* root, Node* nil) { 
            this->root = root; 
            this->nil = nil;
        };
        void ins(long x);
        void del(long x);
        bool find(long x);
        bool checkInv();
        Node* balL1(Node* current);
        Node* balL2(Node* current);
        Node* balR1(Node* current);
        Node* balR2(Node* current);
        Node* getRoot();
        void free();
        long min();
        long max();

    private:
        Node* root;
        Node* nil;
        bool balance(Node* current, Node* parent);
};

}
