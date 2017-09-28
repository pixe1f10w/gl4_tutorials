#include <deps/testing.h/testing.h>
#include <common/matrix.hpp>

BEGIN_TEST()
    math::mat2f m1({1, 2, 3, 4});
    EXPECT_EQUAL(m1.container().size(), 4);
    EXPECT_EQUAL(m1.submatrix(0, 0).container().size(), 1);
    EXPECT_EQUAL(m1.submatrix(1, 1).container().at(0), 1);
    auto& sm = m1.submatrix(0, 0);
    EXPECT_TRUE(sm == sm);
    EXPECT_EQUAL(m1.determinant(), -2);

    math::linear_square_array<3, uint8_t> data({
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });
    data.swap({0, 0}, {2, 2});
    EXPECT_TRUE(data == math::linear_square_array<3, uint8_t>({9, 2, 3, 4, 5, 6, 7, 8, 1}));
END_TEST()
