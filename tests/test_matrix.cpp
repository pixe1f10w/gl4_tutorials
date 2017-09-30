#include <deps/testing.h/testing.h>
#include <common/matrix.hpp>

BEGIN_TEST()
    math::mat2f m1({1, 2, 3, 4});
    //EXPECT_EQUAL(m1.container().size(), 4);
    //EXPECT_EQUAL(m1.submatrix(0, 0).container().size(), 1);
    EXPECT_EQUAL(m1.submatrix(1, 1).container().at(0), 1);
    auto& sm = m1.submatrix(0, 0);
    EXPECT_TRUE(sm == sm);
    EXPECT_EQUAL(m1.determinant(), -2);
END_TEST()
