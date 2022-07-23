
#include "partial_allocator.h"
#include "full_allocator.h"
#include "logging_allocator_mixin.h"
#include "sequence_container.h"

#include <map>

int factorial(int i) {
    int res = i > 0 ? i : 1;
    for (; i>0; --i) {
        res *= i;
    }
    return res;
}

int main() {
    std::map<int, int, std::less<int>, with_logging_allocator<PartialAllocator<std::pair<const int, int>, 10>>> map_partial;
    for (int i = 0; i < 10; ++i) {
        map_partial[i] = factorial(i);
    }

    std::map<int, int, std::less<int>, with_logging_allocator<FullAllocator<std::pair<const int, int>, 10>>> map_full;
    for (int i = 0; i < 10; ++i) {
        map_full[i] = factorial(i);
    }

    SequenceContainer<int> sq;
    sq.push_back(1);
    sq.push_back({ 2, 3 });

    return 0;
}
