#pragma once

#include <math.h>
#include "linear_square_array.hpp"
#include "vector.hpp"

namespace math {

class matrix_error : public std::runtime_error {
public:
    matrix_error(const std::string& error):
        std::runtime_error{error} {}
};

template <size_t dimensions, typename value_type> class matrix;

template<size_t dimensions, typename value_type>
struct determinator {
    static value_type compute(const matrix<dimensions, value_type>& src) {
        value_type result = 0;
        for (size_t i = dimensions; i--;) {
            result += src.container().at(i) * src.cofactor(0, i);
        }
        return result;
    }
};

template<typename value_type>
struct determinator<1, value_type> {
    static value_type compute(const matrix<1, value_type>& src) {
        return src.container().at(0);
    }
};

template <size_t dimensions, typename value_type>
class matrix {

public:
    using container_type = math::linear_square_array<dimensions, value_type>;

    matrix() = default;
    matrix(std::initializer_list<value_type> list):
        m_data{list} {}
    matrix(const container_type& data):
        m_data{data} {}

    const matrix identity() const {
        container_type data;
        for (size_t i = dimensions; i--;) {
            for (size_t j = dimensions; j--;) {
                data[{i, j}] = (i == j);
            }
        }
        return matrix{data};
    }

    const matrix<dimensions - 1, value_type> submatrix(const size_t row, const size_t col) const {
        using result_type = matrix<dimensions - 1, value_type>;
        using result_container_type = typename result_type::container_type;

        result_container_type data;
#if 1
        size_t k = 0;

        for (size_t i = 0; i < dimensions; i++) {
            for (size_t j = 0; j < dimensions; j++) {
                if (i == row || j == col) {
                    continue;
                }
                data[k++] = m_data.at({i, j});
            }
        }
#else
        for (size_t i = dimensions - 1; i--;) {
            for (size_t j = dimensions - 1; j--;) {
                data[{i, j}] = m_data.at({i < row ? i : i + 1, j < col ? j : j + 1});
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

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[{row, col}] = cofactor(row, col);
            }
        }
        // TODO: better way
        return matrix{data}.transpose();
    }

    const matrix invert() const {
        value_type det = determinant();
        if (det == 0) {
            throw matrix_error("matrix with determinant == 0 cannot be inverted");
        }
        return adjugate() * (1 / det);
    }

    const matrix transpose() {
        container_type data(m_data);

        for (size_t row = 0; row < dimensions - 1; row++) {
            for (size_t col = dimensions - 1; col > row; col--) {
                data.swap({row, col}, {col, row});
            }
        }
        return matrix{data};
    }

    bool operator==(const matrix& rhs) const {
        return m_data == rhs.container();
    }

    const matrix operator*(const value_type rhs) const {
        container_type data(m_data);

        for (size_t index = 0; index < dimensions * dimensions; index++) {
            data[index] *= rhs;
        }
        return matrix{data};
    }

    const vector::vector<value_type, dimensions> operator*(const vector::vector<value_type, dimensions>& vec) const {
        typename vector::vector<value_type, dimensions>::container_type data;

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                data[col] += m_data.at({row, col}) * vec.data().at(col);
            }
        }
        return vector::vector<value_type, dimensions>{data};
    }

    const matrix operator*(const matrix& rhs) const {
        container_type data;

        for (size_t row = 0; row < dimensions; row++) {
            for (size_t col = 0; col < dimensions; col++) {
                for (size_t i = 0; i < dimensions; i++) {
                    data[{row, col}] += m_data.at({row, i}) * rhs.container().at({i, col});
                }
            }
        }
        return matrix{data};
    }

    const container_type& container() const {
        return m_data;
    }

protected:

    container_type   m_data;
};

template <size_t dimensions, class value_type> std::ostream& operator<<(std::ostream& out, const matrix<dimensions, value_type>& rhs) {
    out << rhs.container();
    return out;
}

using mat2f = matrix<2, float>;
using mat3f = matrix<3, float>;
using mat4f = matrix<4, float>;

class identity4f : public mat4f {
public:
    identity4f():
        mat4f(mat4f::identity()) {}
};

//TODO: template-based
class translate : public identity4f {
public:
    translate(const float x, const float y, const float z):
        identity4f() {
        m_data[12] = x;
        m_data[13] = y;
        m_data[14] = z;
    }
    translate(const vector::vec3& vec):
        identity4f() {
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

