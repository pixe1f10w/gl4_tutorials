#pragma once

#include <array>
#include <cassert>

namespace math {

template <size_t width, typename value_type>
class linear_square_array {

public:
    static constexpr size_t container_size = width * width;
    using container_type = std::array<value_type, container_size>;
    using index_pair = std::pair<size_t, size_t>;

    linear_square_array() {
        m_container.fill(0);
    }
    // TODO: rewrite without copying (std::forward somehow)
    linear_square_array(std::initializer_list<value_type> list) {
        assert(m_container.size() == list.size());
        size_t index = 0;
        for (auto&& item : list) {
            m_container[index++] = item;
        }
    }
    linear_square_array(const container_type& container):
        m_container(container) {}

    value_type& operator[](const index_pair& pair) {
        return m_container[to_linear_index(pair)];
    }

    value_type& operator[](const size_t linear_index) {
        assert(linear_index < container_size);
        return m_container[linear_index];
    }

    const value_type& at(const index_pair& pair) const {
        return m_container.at(to_linear_index(pair));
    }

    const value_type& at(const size_t linear_index) const {
        assert(linear_index < container_size);
        return m_container.at(linear_index);
    }

    bool operator==(const linear_square_array& other) const {
        return m_container == other.m_container;
    }

    void swap(const index_pair& first, const index_pair& second) {
        value_type temp = (*this)[first];
        (*this)[first]  = (*this)[second];
        (*this)[second] = temp;
    }

    const value_type* raw() const {
        return m_container.data();
    }

private:
    size_t to_linear_index(const size_t row, const size_t col) const {
        return row * width + col;
    }

    size_t to_linear_index(const index_pair& pair) const {
        return to_linear_index(pair.first, pair.second);
    }

    container_type m_container;
};

} // ns math

