#pragma once

#define USE_PRETTY 1
#include "pretty.h"

#include <iostream>

/**
 * Example of full implementation of allocator.
 * Can be used without std::allocator_traits.
 */
template<typename T, std::size_t SIZE>
struct FullAllocator {
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = FullAllocator<U, SIZE>;
    };

    FullAllocator() = default;
    ~FullAllocator() = default;

    template<typename U>
    FullAllocator(const FullAllocator<U, SIZE>&) {
    }

    T *allocate(std::size_t n) {
        std::cout << pretty_function << "[n = " << n << "]" << std::endl;
        auto p = std::malloc(n * sizeof(T));
        if (!p)
            throw std::bad_alloc();
        return reinterpret_cast<T *>(p);
    }

    void deallocate(T *p, std::size_t n) {
        std::cout << pretty_function  << "[n = " << n << "]" << std::endl;
        std::free(p);
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args) {
        std::cout << pretty_function << std::endl;
        new(p) U(std::forward<Args>(args)...);
    };

    void destroy(T *p) {
        std::cout << pretty_function << std::endl;
        p->~T();
    }
};

