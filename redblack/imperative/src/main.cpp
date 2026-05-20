#include "rbt.h"

int main() {
    rbt<int> t{1};
    t.insert(5);
    t.insert(3);
    t.insert(7);
    return 0;
}