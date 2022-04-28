#pragma once

#include <vector>
#include <iostream>

/**
 * Grow-only byte buffer
 */
struct BytesAllocator {
    explicit BytesAllocator(std::size_t size) {
        m_buffer.resize(size); // allocate buffer in free store
    }

    void *allocateBytes(std::size_t size) {
        auto res = tryAllocateBytes(size);
        if (!res) {
            throw std::bad_alloc{};
        }
        return res;
    }

    // may return nullptr
    void *tryAllocateBytes(std::size_t size) {
        if (size > m_buffer.size() - bytesAllocated) {
            return nullptr;
        }
        void *res = m_buffer.data() + bytesAllocated;
        bytesAllocated += size;
        return res;
    }

    void deallocateBytes(void* p, std::size_t size) {
        if (p) {
            // If data are at the end of buffer, we can make it free back.
            // ATTENTION!!! We assume that size equals the value passed corresponding call of allocateBytes
            // In more robust implementations (tolerant to double free errors) we should add control block into every allocation
            if (reinterpret_cast<unsigned char *>(p) + size == m_buffer.data() + bytesAllocated) {
                bytesAllocated -= size;
            }
        }
    }

    std::size_t size() const { return bytesAllocated; }
    std::size_t sizeReserved() const {return m_buffer.size(); }
private:
    std::vector<unsigned char> m_buffer;
    std::size_t bytesAllocated;
};

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
BytesAllocator MyAllocator<T, SIZE>::bytesAllocator(sizeof(T) * SIZE);



template <std::size_t SIZE, class T, class U>
constexpr bool operator== (const MyAllocator<T, SIZE>&, const MyAllocator<U, SIZE>&) noexcept {
    return false;
}

template <std::size_t SIZE, class T, class U>
constexpr bool operator!= (const MyAllocator<T, SIZE>&, const MyAllocator<U, SIZE>&) noexcept {
    return false;
}
