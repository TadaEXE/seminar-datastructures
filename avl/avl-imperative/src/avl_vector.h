#include "avl.h"
#include <iostream>
#include <vector>

class VectorNode {
    public:
        int key;
        int height;
        int left;
        int right;
};

namespace avl {

class AvlVec: public Avl {
    public:
        AvlVec() {};
        void ins(int x);
        void del(int x);
        bool find(int x);
        void free();
        void compareSizes();

        bool checkInv();
        void printLimit();

        std::vector<VectorNode>* getNodes();
        void setSize() { 
            this->size = this->nodes.size(); 
            this->root = 0;
        }

        int min();
        int max();

    private:
        std::vector<VectorNode> nodes;
        int size = 0;
        int root = -1;
        bool balance(int current, int parent);
        void compress();
        void addNode(int x);

        int balL1(int current);
        int balL2(int current);
        int balR1(int current);
        int balR2(int current);
};

}