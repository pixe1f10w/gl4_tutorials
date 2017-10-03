#include <deps/testing.h/testing.h>
#include <common/matrix.hpp>

BEGIN_TEST()
    math::identity4f identity;
    math::mat4f identity_standart = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    EXPECT_EQUAL(identity, identity_standart);

    // basic matrices
    math::mat2f m2 = {
        1, 2,
        3, 4
    };

    math::mat3f m3 = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    // transposed matrices
    math::mat2f m2t = {
        1, 3,
        2, 4
    };
    EXPECT_EQUAL(m2.transpose(), m2t);

    math::mat3f m3t = {
        1, 4, 7,
        2, 5, 8,
        3, 6, 9
    };
    EXPECT_EQUAL(m3.transpose(), m3t);

    // submatrices
    math::mat2f m3_sm00 = {
        5, 6,
        8, 9
    };
    EXPECT_EQUAL(m3.submatrix(0, 0), m3_sm00);

    math::mat2f m3_sm11 = {
        1, 3,
        7, 9
    };
    EXPECT_EQUAL(m3.submatrix(1, 1), m3_sm11);

    // determinants
    EXPECT_EQUAL(m2.determinant(), -2);
    EXPECT_EQUAL(m3.determinant(), 0);

    // cofactors
    EXPECT_EQUAL(m2.cofactor(0, 1), -3);
    EXPECT_EQUAL(m3.cofactor(2, 2), -3);

    // adjugates
    math::mat2f m2a = {
        4, -2,
        -3, 1
    };
    EXPECT_EQUAL(m2.adjugate(), m2a);

    math::mat3f m3a = {
        -3, 6, -3,
        6, -12, 6,
        -3, 6, -3
    };
    EXPECT_EQUAL(m3.adjugate(), m3a);

    // invertions
    math::mat2f m2i = {
        -2, 1,
        1.5, -0.5
    };
    EXPECT_EQUAL(m2.invert(), m2i);
    EXPECT_EQUAL(m2 * m2.invert(), m2.identity());
    EXPECT_EQUAL(m2.invert() * m2, m2.identity());

    // det == 0 -> inversion impossible
    EXPECT_EXCEPTION(m3.invert(), math::matrix_error);

    // operations
    math::mat2f m2x2 = {
        2, 4,
        6, 8
    };
    EXPECT_EQUAL(m2 * 2, m2x2);

    math::mat3f m3x3 = {
        3, 6, 9,
        12, 15, 18,
        21, 24, 27
    };
    EXPECT_EQUAL(m3 * 3, m3x3);

    // TODO: vector

    math::mat2f m2mult = {
        7, 10,
        15, 22
    };
    EXPECT_EQUAL(m2 * m2, m2mult);

    math::mat3f m3mult = {
        30, 36, 42,
        66, 81, 96,
        102, 126, 150
    };
    EXPECT_EQUAL(m3 * m3, m3mult);

    // TODO: special matrices
END_TEST()
