#pragma once

#include <stdint.h>

#define BIT(n) (1ul<<(n))

/*
 * MASK_UNSAFE(3) -> 0b00111
 * MASK_UNSAFE(4) -> 0b01111
 * MASK_UNSAFE(5) -> 0b11111
 * etc...
 */
#define MASK_UNSAFE(x) ((BIT(x) - 1ul))

/**
 * Find Last Set bit in given uint32_t value i.e. find the bit index of the
 * Most Significant Bit (MSB) that is set in the value.
 * @param x
 * @return 0 - 32 are only possible answers given uint32_t is 32 bits.
 */
uint_fast8_t arith_find_most_sig_bit_set(uint32_t x);
