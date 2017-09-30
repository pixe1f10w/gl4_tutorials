#include <deps/testing.h/testing.h>
#include <common/linear_square_array.hpp>

BEGIN_TEST()
    using lsa3 = math::linear_square_array<3, uint8_t>;
    lsa3 data = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };
    // index conversion
    EXPECT_TRUE(data[{1, 1}] == data[4]);

    data.swap({0, 0}, {2, 2});
    lsa3 after_swap = {
        9, 2, 3,
        4, 5, 6,
        7, 8, 1
    };
    EXPECT_TRUE(data == after_swap);
END_TEST()
