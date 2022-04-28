
#include "partial_allocator.h"
#include "full_allocator.h"
#include "logging_allocator_mixin.h"

#include <vector>

int main() {
    std::vector<int, logging_allocator_mixin<int, PartialAllocator<int, 100>>> v_partial;
    for (int i = 0; i < 10; ++i) {
        v_partial.push_back(i);
    }

    std::vector<int, logging_allocator_mixin<int, FullAllocator<int, 100>>> v_full;
    for (int i = 0; i < 10; ++i) {
        v_full.push_back(i);
    }

    return 0;
}
