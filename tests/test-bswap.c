/** Copyright (c) 2026-2036, Jin.Wu <wujin.developer@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include "xylem.h"

// Helper to extract bit representation of float
static inline uint32_t float_bits(float f) {
    uint32_t u;
    memcpy(&u, &f, sizeof(u));
    return u;
}

// Helper to extract bit representation of double
static inline uint64_t double_bits(double d) {
    uint64_t u;
    memcpy(&u, &d, sizeof(u));
    return u;
}

/**
 * @brief Test byte-swapping of a 16-bit unsigned integer with a typical value.
 *
 * Verifies that xylem_bswap correctly reverses the byte order of a uint16_t.
 * Input: 0x1234 ¡ú Expected output: 0x3412.
 */
static void test_bswap_uint16_typical(void) {
    uint16_t input = 0x1234U;
    uint16_t expected = 0x3412U;
    assert(xylem_bswap(input) == expected);
}

/**
 * @brief Test byte-swapping of a 16-bit signed integer at boundary value -1.
 *
 * Ensures correct handling of signed integers by testing the all-ones pattern
 * (0xFFFF), which represents -1 in two's complement. Byte swap should yield
 * the same bit pattern and thus the same value.
 */
static void test_bswap_int16_boundary(void) {
    int16_t input = -1; // Bit pattern: 0xFFFF
    int16_t expected = -1;
    assert(xylem_bswap(input) == expected);
}

/**
 * @brief Test byte-swapping of a 32-bit unsigned integer with a multi-byte
 * pattern.
 *
 * Validates 4-byte reversal: 0x12345678 ¡ú 0x78563412.
 * This covers the core logic for 32-bit word swapping.
 */
static void test_bswap_uint32_typical(void) {
    uint32_t input = 0x12345678U;
    uint32_t expected = 0x78563412U;
    assert(xylem_bswap(input) == expected);
}

/**
 * @brief Test byte-swapping of a 64-bit unsigned integer with asymmetric bytes.
 *
 * Uses a non-palindromic 8-byte pattern to ensure full reversal:
 * 0x0123456789ABCDEF ¡ú 0xEFCDAB8967452301.
 * Tests correctness across all 8 byte positions.
 */
static void test_bswap_uint64_typical(void) {
    uint64_t input = 0x0123456789ABCDEFULL;
    uint64_t expected = 0xEFCDAB8967452301ULL;
    assert(xylem_bswap(input) == expected);
}

/**
 * @brief Test byte-swapping of a 32-bit float via bit-pattern validation.
 *
 * Since floating-point values cannot be compared by numeric equality after
 * byte swapping, this test verifies that the underlying bit representation
 * is correctly reversed using memcpy and uint32_t comparison.
 */
static void test_bswap_float_bitpattern(void) {
    float    input = 123.456f;
    uint32_t original_bits = float_bits(input);
    float    swapped = xylem_bswap(input);
    uint32_t swapped_bits = float_bits(swapped);
    assert(swapped_bits == xylem_bswap_u32(original_bits));
}

/**
 * @brief Test byte-swapping of a double by verifying bit-pattern correctness.
 *
 * Since byte swapping may destroy the IEEE 754 NaN structure (by moving
 * exponent bits out of position), we do NOT assert that the result is NaN.
 * Instead, we verify that the underlying bits were correctly reversed,
 * which is the sole responsibility of xylem_bswap.
 */
static void test_bswap_double_nan(void) {
    double   input = NAN;
    uint64_t original_bits = double_bits(input);
    double   swapped = xylem_bswap(input);
    uint64_t swapped_bits = double_bits(swapped);
    assert(swapped_bits == xylem_bswap_u64(original_bits));
}

/**
 * @brief Test zero-value consistency across all supported types.
 *
 * Zero has the same representation regardless of endianness or sign.
 * This test ensures that byte-swapping zero yields zero for every type,
 * providing a lightweight sanity check for all code paths.
 */
static void test_bswap_zero_all_types(void) {
    assert(xylem_bswap((uint16_t)0) == 0);
    assert(xylem_bswap((int16_t)0) == 0);
    assert(xylem_bswap((uint32_t)0U) == 0U);
    assert(xylem_bswap((int32_t)0) == 0);
    assert(xylem_bswap((uint64_t)0ULL) == 0ULL);
    assert(xylem_bswap((int64_t)0) == 0);
    assert(xylem_bswap(0.0f) == 0.0f);
    assert(xylem_bswap(0.0) == 0.0);
}

int main(void) {
    test_bswap_uint16_typical();
    test_bswap_int16_boundary();
    test_bswap_uint32_typical();
    test_bswap_uint64_typical();
    test_bswap_float_bitpattern();
    test_bswap_double_nan();
    test_bswap_zero_all_types();

    return 0;
}