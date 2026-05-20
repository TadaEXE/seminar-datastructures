#include "rbt.h"

int main() {
    rbt<int> t{1};
    t.insert(7);
    t.insert(3);
    t.insert(2);
    t.insert(100);
    t.insert(50);
    t.insert(0);
    return 0;
}