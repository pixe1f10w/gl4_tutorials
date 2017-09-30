#include <deps/testing.h/testing.h>
#include <common/matrix.hpp>

BEGIN_TEST()
    math::mat2f m1({1, 2, 3, 4});
    auto m2(m1);
    math::mat2f after_transpose = {
        1, 3,
        2, 4
    };
    m2.transpose();
    EXPECT_TRUE(m2 == after_transpose);
    math::mat3f m3 = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    math::mat3f m3t = {
        1, 4, 7,
        2, 5, 8,
        3, 6, 9
    };
    m3.transpose();
    //EXPECT_TRUE(m3 == m3t);
    EXPECT_EQUAL(m3, m3t);
    //EXPECT_TRUE(m2 == m1);
    //EXPECT_EQUAL(m1.container().size(), 4);
    //EXPECT_EQUAL(m1.submatrix(0, 0).container().size(), 1);
    EXPECT_EQUAL(m1.submatrix(1, 1).container().at(0), 1);
    auto& sm = m1.submatrix(0, 0);
    EXPECT_TRUE(sm == sm);
    EXPECT_EQUAL(m1.determinant(), -2);
END_TEST()
