#pragma once

#include <array>
#include <cassert>
#include <math.h>
#include "linear_square_array.hpp"
#include "vector.hpp"

namespace math {

template <size_t dimensions, typename value_type> class matrix;

template<size_t dimensions, typename value_type>
struct determinator {
    static value_type compute(const matrix<dimensions, value_type>& src) {
        value_type result = 0;
        for (size_t i = dimensions; i--;) {
            result += src.m_data.at(i) * src.cofactor(0, i);
        }
        return result;
    }
};

template<typename value_type>
struct determinator<1, value_type> {
    static value_type compute(const matrix<1, value_type>& src) {
        return src.m_data.at(0);
    }
};

template <size_t dimensions, typename value_type>
class matrix {

public:
    using container_type = std::array<value_type, dimensions * dimensions>;
#if 0
    matrix():
        m_main_diagonal_indices(get_main_diagonal_indices()) {
    }
#else
    matrix() = default;
#endif

    matrix(const container_type& data):
        m_main_diagonal_indices(get_main_diagonal_indices()),
        m_data(data) {}

    void identity() {
#if 0
        m_data.fill(0);
        for (auto& idx : m_main_diagonal_indices) {
            m_data[idx] = 1;
        }
#else
        for (size_t i = dimensions; i--;) {
            for (size_t j = dimensions; j--;) {
                m_data[to_index(i, j)] = (i == j);
            }
        }
#endif
    }

    const matrix<dimensions - 1, value_type> submatrix(const size_t row, const size_t col) const {
        using result_type = matrix<dimensions - 1, value_type>;
        using result_container_type = typename result_type::container_type;

        result_container_type data;
        data.fill(0);
#if 1
        size_t k = 0;

        for (size_t i = 0; i < dimensions; i++) {
            for (size_t j = 0; j < dimensions; j++) {
                if (i == row || j == col) {
                    continue;
                }
                data[k++] = m_data.at(to_index(i, j));
            }
        }
#else
        for (size_t i = dimensions - 1; i--;) {
            for (size_t j = dimensions - 1; j--;) {
                data[to_index(i, j)] = m_data.at(to_index(i < row ? i : i + 1, j < col ? j : j + 1));
            }
        }
#endif
        return result_type{data};
    }

    value_type cofactor(const size_t row, const size_t col) const {
        return ((row + col) % 2 ? -1 : 1) * submatrix(row, col).determinant();
    }

    value_type determinant() const {
        return determinator<dimensions, value_type>::compute(*this);
    }

    const matrix adjugate() const {
        container_type data;
        data.fill(0);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[to_index(row, col)] = cofactor(row, col);
            }
        }
        return matrix(data);
    }

    const matrix operator*(const matrix& other) const {
        container_type data;
        data.fill(0);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                for (size_t i = 0; i < dimensions; i++) {
                    data[to_index(row, col)] += m_data.at(to_index(row, i)) * other.container().at(to_index(i, col));
                }
            }
        }
        return matrix(data);
    }

    bool operator==(const matrix& other) const {
        return m_data == other.container();
    }

    const vector::vector<value_type, dimensions> operator*(const vector::vector<value_type, dimensions>& vec) const {
        typename vector::vector<value_type, dimensions>::container_type data;
        data.fill(0);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[col] += m_data.at(to_index(row, col)) * vec.data().at(col);
            }
        }
        return vector::vector<value_type, dimensions>(data);
    }

    const container_type& container() const {
        return m_data;
    }

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
    container_type   m_data;
};

using mat2f = matrix<2, float>;
using mat3f = matrix<3, float>;
using mat4f = matrix<4, float>;

class identity4f : public mat4f {
public:
    identity4f():
        mat4f() {
        identity();
    }
};

//TODO: template-based
class translate : public identity4f {
public:
    translate(const float x, const float y, const float z):
        identity4f() {
        identity();
        m_data[12] = x;
        m_data[13] = y;
        m_data[14] = z;
    }
    translate(const vector::vec3& vec):
        identity4f() {
        identity();
        m_data[12] = vec.data()[0];
        m_data[13] = vec.data()[1];
        m_data[14] = vec.data()[2];
    }
};

class scale : public identity4f {
public:
    scale(const float x, const float y, const float z):
        identity4f() {
        m_data[0] = x;
        m_data[5] = y;
        m_data[10] = z;
    }
    scale(const vector::vec3& vec):
        identity4f() {
        m_data[0] = vec.data()[0];
        m_data[5] = vec.data()[1];
        m_data[10] = vec.data()[2];
    }
};

class rotate_x : public identity4f {
public:
    rotate_x(const float value):
        identity4f() {
        m_data[5] = cos(value);
        m_data[6] = sin(value);
        m_data[9] = -sin(value);
        m_data[10] = cos(value);
    }
};

class rotate_y : public identity4f {
public:
    rotate_y(const float value):
        identity4f() {
        m_data[0] = cos(value);
        m_data[2] = -sin(value);
        m_data[8] = sin(value);
        m_data[10] = cos(value);
    }
};

class rotate_z : public identity4f {
public:
    rotate_z(const float value):
        identity4f() {
        m_data[0] = cos(value);
        m_data[1] = sin(value);
        m_data[4] = -sin(value);
        m_data[5] = cos(value);
    }
};

} // ns math

