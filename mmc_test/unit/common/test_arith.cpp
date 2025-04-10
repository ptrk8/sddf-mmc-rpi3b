#include <gtest/gtest.h>

extern "C" {
#include "arith.h"
}

/* BIT macro */

TEST(test_bit, should_set_first_bit) {
    ASSERT_EQ(
            0b00000000000000000000000000000001,
            BIT(0)
    );
}

TEST(test_bit, should_set_second_bit) {
    ASSERT_EQ(
            0b00000000000000000000000000000010,
            BIT(1)
    );
}

TEST(test_bit, should_set_third_bit) {
    ASSERT_EQ(
            0b00000000000000000000000000000100,
            BIT(2)
    );
}

TEST(test_bit, should_set_32nd_bit) {
    ASSERT_EQ(
            0b10000000000000000000000000000000,
            BIT(31)
    );
}

TEST(test_bit, should_set_64th_bit) {
    ASSERT_EQ(
            0b1000000000000000000000000000000000000000000000000000000000000000,
            BIT(63)
    );
}

/* arith_find_most_sig_bit_set */

TEST(test_bit, arith_find_most_sig_bit_set_should_return_most_sig_bit_set) {
    for (size_t i = 0; i < 32; i++) {
        ASSERT_EQ(
                arith_find_most_sig_bit_set(BIT(i)),
                i + 1
        );
    }
}

