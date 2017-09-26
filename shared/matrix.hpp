#pragma once

#include <array>
#include <math.h>
#include "vector.hpp"

namespace matrix {

template <typename value_type, size_t size> class matrix;

template<size_t dimensions, typename value_type>
struct determinator {
    static value_type compute(const matrix<value_type, dimensions>& src) {
        value_type result = 0;
        for (size_t i = dimensions; i--;) {
            result += src.m_data.at(i) * src.cofactor(0, i);
        }
        return result;
    }
};

template<typename value_type>
struct determinator<1, value_type> {
    static value_type compute(const matrix<value_type, 1>& src) {
        return src.m_data.at(0);
    }
};

template <typename value_type, size_t size>
class matrix {
public:

    static const size_t dimensions = size;
    using data_type = std::array<value_type, dimensions * dimensions>;

    matrix():
        m_main_diagonal_indices(get_main_diagonal_indices()) {
        identity();
    }

    matrix(const data_type& data):
        m_main_diagonal_indices(get_main_diagonal_indices()),
        m_data(data) {}

    void identity() {
        m_data.fill(0);
        for (auto& idx : m_main_diagonal_indices) {
            m_data[idx] = 1;
        }
    }

    const matrix<value_type, size - 1>& submatrix(const size_t row, const size_t col) const {
        using result_type = matrix<value_type, size - 1>;
        using result_data_type = typename result_type::data_type;

        result_data_type data;
        data.fill(0);
        /*
        size_t k = 0;

        for (size_t i = 0; i < dimensions; i++) {
            for (size_t j = 0; j < dimensions; j++) {
                if (i == row || j == col) {
                    continue;
                }
                data[k++] = m_data.at(to_index(i, j));
            }
        }

        return std::move(result_type{data});
        */
        for (size_t i = dimensions - 1; i--;) {
            for (size_t j = dimensions - 1; j--;) {
                data[to_index(i, j)] = m_data.at(to_index(i < row ? i : i + 1, j < col ? j : j + 1));
            }
        }

        std::cerr << "[";
        for (auto& item : data ) {
            std::cerr << item << ", ";
        }
        std::cerr << "]\n";

        return std::move(result_type{data});
    }

    value_type cofactor(const size_t row, const size_t col) const {
        return ((row + col) % 2 ? -1 : 1) * submatrix(row, col).determinant();
    }

    value_type determinant() const {
        return determinator<dimensions, value_type>::compute(*this);
    }

    const matrix& adjugate() const {
        data_type data;
        data.fill(0);

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[to_index(row, col)] = cofactor(row, col);
            }
        }
        return std::move(matrix(data));
    }

    const matrix& operator*(const matrix& other) const {
        data_type data;
        data.fill(0);

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
        data.fill(0);

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

//protected:
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

