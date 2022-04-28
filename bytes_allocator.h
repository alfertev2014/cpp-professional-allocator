#pragma once

#include <vector>

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
