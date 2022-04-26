#pragma once

#define USE_PRETTY 1
#include "pretty.h"

#include <iostream>

template<typename T, typename Allocator = std::allocator<T>>
struct logging_allocator_mixin {
    using value_type = typename Allocator::value_type;

    logging_allocator_mixin() = default;
    ~logging_allocator_mixin() = default;

    template<typename U>
    logging_allocator_mixin(const logging_allocator_mixin<U, Allocator>&) {

    }

    T *allocate(std::size_t n) {
        std::cout << pretty_function << "[n = " << n << "]" << std::endl;
        return allocator.allocate(n);
    }

    void deallocate(T *p, std::size_t n) {
        std::cout << pretty_function  << "[n = " << n << "]" << std::endl;
        allocator.deallocate(p, n);
    }
private:
    Allocator allocator;
};
