#pragma once

#include <array>

namespace vector {

template <typename value_type, size_t size>
class vector {
public:
    static const size_t dimensions = size;
    using data_type = std::array<value_type, dimensions>;
    data_type m_data;

    vector() {
        m_data.fill(0);
    }

    vector(const data_type& data):
        m_data(data) {}

    const data_type& data() const {
        return m_data;
    }

    const value_type* raw_data() {
        return m_data.data();
    }
};

using vec2 = vector<float, 2>;
using vec3 = vector<float, 3>;
using vec4 = vector<float, 4>;

} // ns vector

