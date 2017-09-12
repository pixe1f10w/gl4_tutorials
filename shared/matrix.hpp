#pragma once

#include <array>
#include <math.h>
#include "vector.hpp"

namespace matrix {

template <typename value_type, size_t size>
class matrix {
protected:

    static const size_t dimensions = size;

    using data_type = std::array<value_type, dimensions * dimensions>;
    using row_indices = std::array<size_t, dimensions>;

    size_t to_index(const size_t row, const size_t col) const {
        return row * dimensions + col;
    }

    //TODO: constexpr somehow?
    row_indices get_main_diagonal_indices() const {
        row_indices indices;
        for (size_t i = 0; i < dimensions; i++) {
            indices[i] = to_index(i, i);
        }
        return std::move(indices);
    }

    row_indices m_main_diagonal_indices;
    data_type   m_data;

public:

    matrix():
        m_main_diagonal_indices(get_main_diagonal_indices()) {
        identity();
    }

    matrix(const data_type& data):
        m_main_diagonal_indices(get_main_diagonal_indices()),
        m_data(data) {}

    void identity() {
        m_data.fill(.0f);
        for (auto& idx : m_main_diagonal_indices) {
            m_data[idx] = 1.0f;
        }
    }

    const matrix& operator*(const matrix& other) const {
        data_type data;
        data.fill(.0f);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                for (size_t i = 0; i < dimensions; i++) {
                    data[to_index(row, col)] += m_data.at(to_index(row, i)) * other.data().at(to_index(i, col));
                }
            }
        }
        return std::move(matrix(data));
    }

    const vector::vector<value_type, size>& operator*(const vector::vector<value_type, size>& vec) const {
        typename vector::vector<value_type, size>::data_type data;
        data.fill(.0f);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[col] += m_data.at(to_index(row, col)) * vec.data().at(col);
            }
        }
        return std::move(vector::vector<value_type, size>(data));
    }

    const data_type& data() const {
        return m_data;
    }

    const value_type* raw_data() {
        return m_data.data();
    }
};

using mat2 = matrix<float, 2>;
using mat3 = matrix<float, 3>;
using mat4 = matrix<float, 4>;

//TODO: template-based
class translate : public mat4 {
public:
    translate(const float x, const float y, const float z):
        mat4() {
        m_data[12] = x;
        m_data[13] = y;
        m_data[14] = z;
    }
    translate(const vector::vec3& vec):
        mat4() {
        m_data[12] = vec.data()[0];
        m_data[13] = vec.data()[1];
        m_data[14] = vec.data()[2];
    }
};

class scale : public mat4 {
public:
    scale(const float x, const float y, const float z):
        mat4() {
        m_data[0] = x;
        m_data[5] = y;
        m_data[10] = z;
    }
    scale(const vector::vec3& vec):
        mat4() {
        m_data[0] = vec.data()[0];
        m_data[5] = vec.data()[1];
        m_data[10] = vec.data()[2];
    }
};

class rotate_x : public mat4 {
public:
    rotate_x(const float value):
        mat4() {
        m_data[5] = cos(value);
        m_data[6] = sin(value);
        m_data[9] = -sin(value);
        m_data[10] = cos(value);
    }
};

class rotate_y : public mat4 {
public:
    rotate_y(const float value):
        mat4() {
        m_data[0] = cos(value);
        m_data[2] = -sin(value);
        m_data[8] = sin(value);
        m_data[10] = cos(value);
    }
};

class rotate_z : public mat4 {
public:
    rotate_z(const float value):
        mat4() {
        m_data[0] = cos(value);
        m_data[1] = sin(value);
        m_data[4] = -sin(value);
        m_data[5] = cos(value);
    }
};

} // ns matrix

