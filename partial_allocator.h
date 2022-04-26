#pragma once

#include <array>
#include <iostream>

/**
 * Example of partial implementation of allocator
 * to be used with std::allocator_traits
 */
template <typename T, std::size_t SIZE>
struct MyAllocator {
    static_assert(SIZE > 0, "SIZE must be greater than 0");

    using value_type = T;

    MyAllocator() = default;
    ~MyAllocator() = default;

    template <typename U>
    MyAllocator(const MyAllocator<U, SIZE>& ) {
        // should we make a copy of the rhs.m_buffer ?
        // No, we should not!
    }

    template<class U>
    struct rebind   // we cannot get rid of this :(
    {
        using other = MyAllocator<U, SIZE>;
    };

    T* allocate (std::size_t n) {
        auto p = allocateBytes(n * sizeof(T));
        if (!p) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<T *>(p);
    }

    void deallocate (T* p, std::size_t n) {
        deallocateBytes(p, n * sizeof(T));
    }
private:
    // Use raw byte buffer because we will not call constructor of T elements right away.
    static std::array<unsigned char, sizeof(T) * SIZE> m_buffer;
    static std::size_t bytesAllocated;

    static void *allocateBytes(std::size_t size) {
        std::cout << "allocate " << size << std::endl;
        if (size > m_buffer.size() - bytesAllocated) {
            throw std::bad_alloc{};
        }
        void *res = m_buffer.data() + bytesAllocated;
        bytesAllocated += size;
        return res;
    }

    static void deallocateBytes(T* p, std::size_t size) {
        std::cout << "deallocate " << size << std::endl;
        if (p) {
            // If data are at the end of buffer, we can make it free back.
            // ATTENTION!!! We assume that size equals the value passed corresponding call of allocateBytes
            // In more robust implementations (tolerant to doublefree) we should add control block in every allocation
            unsigned char *bytes = reinterpret_cast<unsigned char *>(reinterpret_cast<void *>(p));
            if (bytes + size == m_buffer.data() + bytesAllocated) {
                bytesAllocated -= size;
                std::cout << "deallocate last" << std::endl;
            }
        }
    }
};

template <typename T, std::size_t SIZE>
std::array<unsigned char, sizeof(T) * SIZE> MyAllocator<T, SIZE>::m_buffer;
template <typename T, std::size_t SIZE>
std::size_t MyAllocator<T, SIZE>::bytesAllocated;



template <std::size_t SIZE, class T, class U>
constexpr bool operator== (const MyAllocator<T, SIZE>&, const MyAllocator<U, SIZE>&) noexcept {
    return false;
}

template <std::size_t SIZE, class T, class U>
constexpr bool operator!= (const MyAllocator<T, SIZE>&, const MyAllocator<U, SIZE>&) noexcept {
    return false;
}
