#pragma once

#include <memory>
#include <initializer_list>
#include <array>
#include <cinttypes>

// https://en.cppreference.com/w/cpp/named_req/Container
// https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer

// forward linked list of short arrays of T

template <typename T, typename Allocator = std::allocator<T>>
struct SequenceContainer {

    // Like readonly std::vector
    struct Node {
        T *data {};
        std::size_t size {};
        Node *next {};

        Node() = delete;
        Node(const Node &other, const Allocator &allocator = Allocator())
            : Node(other.data, other.data + other.size, allocator) {}
        Node(Node &&other, const Allocator &allocator = Allocator()) noexcept
            : data(other.data), size(other.size), next(other.next) {
                other.data = nullptr;
                other.size = 0;
                other.next = 0;
            }

        Node(const std::initializer_list<T>& list, const Allocator &allocator = Allocator()) {
            size = list.size();
            data = al_traits::allocate(allocator, size);
            std::size_t i = 0;
            for (auto &e : list) {
                al_traits::construct(allocator, data + i, std::move(e));
            }
        }

        template <typename It>
        Node(const It &begin, const It &end, const Allocator &allocator = Allocator()) {
            size = end - begin;
            data = al_traits::allocate(allocator, size);
            std::size_t i = 0;
            for (auto it = begin; it != end; ++it) {
                al_traits::construct(allocator, data + i, *it);
            }
        }

        ~Node() {
            if (data) {
                Allocator allocator {};
                for (T* p = data; p != data + size; ++p) {
                    al_traits::destroy(allocator, p);
                }
                al_traits::deallocate(allocator, size);
                data = nullptr;
                size = 0;
            }
        }

        Node &operator=(const Node& other) { // copy-and-swap idioma
            Node cp {other};
            swap(this, cp);
            return *this;
        }

        Node &operator=(Node&& other) { // move-and-swap idioma
            Node mv {std::move(other)};
            swap(this, mv);
            return *this;
        }

        void swap(Node& other) noexcept {
            std::swap(data, other.data);
            std::swap(size, other.size);
            std::swap(next, other.next);
        }
    };

    struct Iterator {
        Iterator() = default;
        Iterator(Node *node, std::size_t offset)
            : m_node(node), offset(offset) {}

        const T& operator *() const {
            return *const_cast<Iterator*>(this);
        }

        T& operator *() {
            return *(m_node->data + offset);
        }

        Iterator operator++() const {
            if (m_node == nullptr) {
                return {};
            }
            if (offset == m_node->size - 1) {
                return Iterator(m_node->next, 0);
            } else {
                return Iterator(m_node, offset + 1);
            }
        }

        using value_type = T;
        using reference = T&;
        using difference_type = long;
    private:
        Node *m_node {};
        std::size_t offset {};
    };

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterator;
    using const_iterator = const Iterator;
    using size_type = std::size_t;

    using allocator_type = Allocator;


    SequenceContainer() = default;

    SequenceContainer(const SequenceContainer& other)
        : m_allocator{al_traits::select_on_container_copy_construction(other.get_allocator())}
    {
        m_head = al_traits::allocate(m_allocator, other.size());
        m_size = other.size();
    }

    SequenceContainer(SequenceContainer&& other) noexcept
        : m_head{other.m_head}, m_size{other.m_size}, m_allocator{std::move(other.m_allocator)}
    {
        other.m_head = nullptr;
        other.m_size = 0;
    }

    SequenceContainer(const std::initializer_list<T>& list, const Allocator &allocator = Allocator())
        : m_allocator{allocator}
    {

    }

    ~SequenceContainer() {
        while (m_head) {
            Node *next = m_head->next;
            delete m_head;
            m_head = next;
        }
    }

    SequenceContainer &operator=(const SequenceContainer& other) {
        SequenceContainer cp {other};
        swap(this, cp);
        return *this;
    }

    SequenceContainer &operator=(SequenceContainer&& other) {
        SequenceContainer mv {std::move(other)};
        swap(this, mv);
        return *this;
    }

    void swap(SequenceContainer& other) noexcept {
        std::swap(m_head, other.m_head);
        std::swap(m_size, other.m_size);
        std::swap(m_allocator, other.m_allocator);
    }

    const_iterator cbegin() const { return m_head->data; }
    iterator begin() { return const_cast<iterator>(cbegin()); }
    const_iterator cend() const { return Iterator{}; }
    iterator end() { return const_cast<iterator>(cend()); }

    bool operator==(const SequenceContainer& other) const {
        if (m_size != other.m_size) {
            return false;
        }
        for (iterator i = cbegin(), j = other.cbegin(); i != cend(); ++i, ++j) {
            if (*i != *j) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const SequenceContainer& other) const {
        return operator==(other);
    }

    std::size_t size() const { return m_size; }
    std::size_t max_size() const { return 42; }
    bool empty() const { return begin() == end(); }

    Allocator get_allocator() const { return m_allocator; }

    template <typename ...Args>
    void push_back(Args&& ...args) {
        using rb_al_traits = typename al_traits::template rebind<Node>::other;
        Node * newNode = rb_al_traits::allocate(m_allocator, 1);
        rb_al_traits::construct(std::forward(args)...);
        if (!m_head) {
            m_head = newNode;
            m_tail = m_head;
        } else {
            m_tail->next = newNode;
            m_tail = m_tail->next;
        }
        m_size += m_tail->size;
    }
private:
    using al_traits = std::allocator_traits<Allocator>;

    Node *m_head {};
    Node *m_tail {};
    std::size_t m_size {};
    Allocator m_allocator {};
};
