#pragma once

#include "bytes_allocator.h"

/**
 * Example of partial implementation of allocator
 * to be used with std::allocator_traits
 */
template <typename T, std::size_t SIZE>
struct PartialAllocator {
    static_assert(SIZE > 0, "SIZE must be greater than 0");

    using value_type = T;

    PartialAllocator() = default;
    ~PartialAllocator() = default;

    template <typename U>
    PartialAllocator(const PartialAllocator<U, SIZE>& ) {
        // should we make a copy of the rhs.m_buffer ?
        // No, we should not!
    }

    template<class U>
    struct rebind   // we cannot get rid of this :(
    {
        using other = PartialAllocator<U, SIZE>;
    };

    T* allocate (std::size_t n) {
        auto p = bytesAllocator.tryAllocateBytes(n * sizeof(T));
        if (!p) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<T *>(p);
    }

    void deallocate (T* p, std::size_t n) {
        bytesAllocator.deallocateBytes(p, n * sizeof(T));
    }
private:
    static BytesAllocator bytesAllocator;
};

template <typename T, std::size_t SIZE>
BytesAllocator PartialAllocator<T, SIZE>::bytesAllocator(sizeof(T) * SIZE);



template <std::size_t SIZE, class T, class U>
constexpr bool operator== (const PartialAllocator<T, SIZE>&, const PartialAllocator<U, SIZE>&) noexcept {
    return false;
}

template <std::size_t SIZE, class T, class U>
constexpr bool operator!= (const PartialAllocator<T, SIZE>&, const PartialAllocator<U, SIZE>&) noexcept {
    return false;
}
