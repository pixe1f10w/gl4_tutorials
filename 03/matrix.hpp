#pragma once

#include <array>
#include <math.h>

template <typename value_type, size_t size>
class matrix {
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

    //TODO: template-based
    void translate(const value_type x, const value_type y, const value_type z) {
        identity();
        m_data[12] = x;
        m_data[13] = y;
        m_data[14] = z;
    }

    void scale(const value_type x, const value_type y, const value_type z) {
        identity();
        m_data[0] = x;
        m_data[5] = y;
        m_data[10] = z;
    }

    void rotate_x(const value_type value) {
        identity();
        m_data[5] = cos(value);
        m_data[6] = sin(value);
        m_data[9] = -sin(value);
        m_data[10] = cos(value);
    }

    void rotate_y(const value_type value) {
        identity();
        m_data[0] = cos(value);
        m_data[2] = -sin(value);
        m_data[8] = sin(value);
        m_data[10] = cos(value);
    }

    void rotate_z(const value_type value) {
        identity();
        m_data[0] = cos(value);
        m_data[1] = sin(value);
        m_data[4] = -sin(value);
        m_data[5] = cos(value);
    }

    const matrix& operator*(const matrix& other) const {
        data_type data;

        //TODO: seems buggy. column first?
        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                for (size_t i = 0; i < dimensions; i++) {
                    data[to_index(row, col)] += m_data.at(to_index(row, i)) * other.data().at(to_index(i, col));
                }
            }
        }
        return matrix(data);
    }

    const data_type& data() const {
        return m_data;
    }

    const value_type* raw_data() {
        return m_data.data();
    }
};

using mat4 = matrix<float, 4>;
