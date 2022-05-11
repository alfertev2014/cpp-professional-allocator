#pragma once

#define USE_PRETTY 1
#include "pretty.h"

#include <iostream>
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
struct logging_allocator_mixin : public Allocator {
    using value_type = typename Allocator::value_type;
    using al_traits = std::allocator_traits<Allocator>;

    logging_allocator_mixin() = default;
    ~logging_allocator_mixin() = default;

    template<class U>
    struct rebind   // we cannot get rid of this :(
    {
        using other = logging_allocator_mixin<U, typename al_traits::template rebind_alloc<U>>;
    };

    template<typename U>
    logging_allocator_mixin(const logging_allocator_mixin<U, typename al_traits::template rebind_alloc<U>>&) {

    }

    typename al_traits::pointer allocate(std::size_t n) {
        std::cout << pretty_function << "[n = " << n << "]" << std::endl;
        return Allocator::allocate(n);
    }

    void deallocate(typename al_traits::pointer p, std::size_t n) {
        std::cout << pretty_function  << "[n = " << n << "]" << std::endl;
        Allocator::deallocate(p, n);
    }

};

template<typename Allocator>
using with_logging_allocator = logging_allocator_mixin<typename Allocator::value_type, Allocator>;
