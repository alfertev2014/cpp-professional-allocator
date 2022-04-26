
#include "partial_allocator.h"
#include "logging_allocator_mixin.h"

#include <vector>

int main() {
    std::vector<int, logging_allocator_mixin<int, MyAllocator<int, 100>>> v;
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
    return 0;
}
