#include "rbt.h"

int main() {
    rbt<int>* t = new rbt<int>(1);
    t->insert(7);
    t->insert(3);
    t->insert(2);
    t->insert(100);
    t->insert(50);
    t->insert(0);
    t->insert(-12);
    t->insert(-100);
    t->insert(32);
    t->insert(201);
    t->insert(5);

    t->delete_node(2);

    rbt<int>* t2 = new rbt<int>(15);
    t2->insert(20);
    t2->insert(0);
    t2->insert(-1);

    t2->delete_node(0);

    std::cout << "Lookup t 100:" << t2->lookup(20) << std::endl;
    std::cout << "Lookup t 2:" << t2->lookup(0) << std::endl;
    std::cout << "Lookup t2 20:" << t2->lookup(20) << std::endl;
    std::cout << "Lookup t2 0:" << t2->lookup(0) << std::endl;

    return 0;
}