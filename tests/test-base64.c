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
#include "ASSERT.h"

/**
 * Test encoding and decoding of empty input (zero-length).
 * Both standard and URL-safe variants should handle it gracefully.
 */
static void test_empty_input(void) {
    uint8_t enc[10], dec[10];

    // Standard encode/decode
    int elen = xylem_base64_encode_std(NULL, 0, enc, sizeof(enc));
    ASSERT(elen == 0);

    int dlen = xylem_base64_decode_std(NULL, 0, dec, sizeof(dec));
    ASSERT(dlen == 0);

    // URL-safe (with and without padding)
    elen = xylem_base64_encode_url(NULL, 0, enc, sizeof(enc), true);
    ASSERT(elen == 0);
    elen = xylem_base64_encode_url(NULL, 0, enc, sizeof(enc), false);
    ASSERT(elen == 0);

    dlen = xylem_base64_decode_url(NULL, 0, dec, sizeof(dec), true);
    ASSERT(dlen == 0);
    dlen = xylem_base64_decode_url(NULL, 0, dec, sizeof(dec), false);
    ASSERT(dlen == 0);
}

/**
 * Test 1-byte input: "A" (0x41) ¡ú "QQ==" (std) or "QQ" (url, no pad).
 * Verifies partial block handling and padding logic.
 */
static void test_one_byte(void) {
    uint8_t input[] = {0x41};
    uint8_t enc[10], dec[10];

    // Standard: must produce "QQ=="
    int elen = xylem_base64_encode_std(input, 1, enc, sizeof(enc));
    ASSERT(elen == 4);
    ASSERT(memcmp(enc, "QQ==", 4) == 0);

    int dlen = xylem_base64_decode_std(enc, 4, dec, sizeof(dec));
    ASSERT(dlen == 1);
    ASSERT(dec[0] == 0x41);

    // URL with padding
    elen = xylem_base64_encode_url(input, 1, enc, sizeof(enc), true);
    ASSERT(elen == 4);
    ASSERT(memcmp(enc, "QQ==", 4) == 0);
    dlen = xylem_base64_decode_url(enc, 4, dec, sizeof(dec), true);
    ASSERT(dlen == 1 && dec[0] == 0x41);

    // URL without padding
    elen = xylem_base64_encode_url(input, 1, enc, sizeof(enc), false);
    ASSERT(elen == 2);
    ASSERT(memcmp(enc, "QQ", 2) == 0);
    dlen = xylem_base64_decode_url(enc, 2, dec, sizeof(dec), false);
    ASSERT(dlen == 1 && dec[0] == 0x41);
}

/**
 * Test 2-byte input: "AB" (0x41,0x42) ¡ú "QUI=" (std) or "QUI" (url, no pad).
 */
static void test_two_bytes(void) {
    uint8_t input[] = {0x41, 0x42};
    uint8_t enc[10], dec[10];

    int elen = xylem_base64_encode_std(input, 2, enc, sizeof(enc));
    ASSERT(elen == 4);
    ASSERT(memcmp(enc, "QUI=", 4) == 0);

    int dlen = xylem_base64_decode_std(enc, 4, dec, sizeof(dec));
    ASSERT(dlen == 2);
    ASSERT(dec[0] == 0x41 && dec[1] == 0x42);

    // URL without padding
    elen = xylem_base64_encode_url(input, 2, enc, sizeof(enc), false);
    ASSERT(elen == 3);
    ASSERT(memcmp(enc, "QUI", 3) == 0);
    dlen = xylem_base64_decode_url(enc, 3, dec, sizeof(dec), false);
    ASSERT(dlen == 2);
    ASSERT(dec[0] == 0x41 && dec[1] == 0x42);
}

/**
 * Test 3-byte input: full block ("ABC") ¡ú "QUJD", no padding.
 */
static void test_three_bytes(void) {
    uint8_t input[] = {0x41, 0x42, 0x43};
    uint8_t enc[10], dec[10];

    int elen = xylem_base64_encode_std(input, 3, enc, sizeof(enc));
    ASSERT(elen == 4);
    ASSERT(memcmp(enc, "QUJD", 4) == 0);

    int dlen = xylem_base64_decode_std(enc, 4, dec, sizeof(dec));
    ASSERT(dlen == 3);
    ASSERT(memcmp(dec, input, 3) == 0);

    // URL mode (same output since no padding needed)
    elen = xylem_base64_encode_url(input, 3, enc, sizeof(enc), false);
    ASSERT(elen == 4);
    ASSERT(memcmp(enc, "QUJD", 4) == 0);
    dlen = xylem_base64_decode_url(enc, 4, dec, sizeof(dec), false);
    ASSERT(dlen == 3);
    ASSERT(memcmp(dec, input, 3) == 0);
}

/**
 * Test multi-block input: "Hello!" (6 bytes) ¡ú "SGVsbG8h".
 */
static void test_multi_block(void) {
    uint8_t input[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x21}; // "Hello!"
    uint8_t enc[20], dec[20];

    int elen = xylem_base64_encode_std(input, 6, enc, sizeof(enc));
    ASSERT(elen == 8);
    ASSERT(memcmp(enc, "SGVsbG8h", 8) == 0);

    int dlen = xylem_base64_decode_std(enc, 8, dec, sizeof(dec));
    ASSERT(dlen == 6);
    ASSERT(memcmp(dec, input, 6) == 0);
}

/**
 * Test decoding with illegal characters (e.g., '!').
 * Should return -1 for both std and url modes when char is invalid.
 */
static void test_illegal_characters(void) {
    uint8_t dec[10];

    // Standard: '!' is invalid
    ASSERT(
        xylem_base64_decode_std((uint8_t*)"AB!D", 4, dec, sizeof(dec)) == -1);

    // URL: '+' is invalid in URL mode
    ASSERT(
        xylem_base64_decode_url((uint8_t*)"AB+D", 4, dec, sizeof(dec), false) ==
        -1);

    // But '_' is valid in URL
    int dlen =
        xylem_base64_decode_url((uint8_t*)"AB_D", 4, dec, sizeof(dec), false);
    ASSERT(dlen >= 0); // should succeed
}

/**
 * Test malformed padding: e.g., "Q=Q=", "=QQQ", "Q===".
 * All should be rejected by standard decoder.
 */
static void test_malformed_padding(void) {
    uint8_t dec[10];

    ASSERT(
        xylem_base64_decode_std((uint8_t*)"Q=Q=", 4, dec, sizeof(dec)) == -1);
    ASSERT(
        xylem_base64_decode_std((uint8_t*)"=QQQ", 4, dec, sizeof(dec)) == -1);
    ASSERT(
        xylem_base64_decode_std((uint8_t*)"Q===", 4, dec, sizeof(dec)) == -1);
}

/**
 * Test non-multiple-of-4 input in standard mode: should fail.
 * But allowed in URL mode without padding.
 */
static void test_non_multiple_of_4(void) {
    uint8_t dec[10];

    // Standard mode: reject
    ASSERT(xylem_base64_decode_std((uint8_t*)"QQ", 2, dec, sizeof(dec)) == -1);
    ASSERT(xylem_base64_decode_std((uint8_t*)"QUI", 3, dec, sizeof(dec)) == -1);

    // URL mode without padding: accept
    ASSERT(
        xylem_base64_decode_url((uint8_t*)"QQ", 2, dec, sizeof(dec), false) >=
        0);
    ASSERT(
        xylem_base64_decode_url((uint8_t*)"QUI", 3, dec, sizeof(dec), false) >=
        0);

    // URL mode with padding: reject (not multiple of 4)
    ASSERT(
        xylem_base64_decode_url((uint8_t*)"QQ", 2, dec, sizeof(dec), true) ==
        -1);
}

/**
 * Test buffer size too small for encoding or decoding.
 * Should return -1 without writing beyond bounds.
 */
static void test_insufficient_buffer(void) {
    uint8_t input[] = {0x41};
    uint8_t tiny_enc[3]; // needs 4
    uint8_t tiny_dec[2]; // needs 3 for decode of 4-char input

    // Encode: dlen=3 < required 4 ¡ú -1
    ASSERT(xylem_base64_encode_std(input, 1, tiny_enc, 3) == -1);

    // Decode: dlen=2 < ((4+3)/4)*3 = 3 ¡ú -1
    ASSERT(xylem_base64_decode_std((uint8_t*)"QQ==", 4, tiny_dec, 2) == -1);
}

/**
 * Round-trip test for input lengths 0 to 10 bytes.
 * Ensures encode ¡ú decode recovers original data exactly.
 */
static void test_round_trip(void) {
    uint8_t input[11], enc[32], dec[16];

    for (int n = 0; n <= 10; n++) {
        // Fill with deterministic pattern
        for (int i = 0; i < n; i++) {
            input[i] = (uint8_t)i;
        }

        // Standard round-trip
        int elen = xylem_base64_encode_std(input, n, enc, sizeof(enc));
        ASSERT(elen >= 0);
        int dlen = xylem_base64_decode_std(enc, elen, dec, sizeof(dec));
        ASSERT(dlen == n);
        ASSERT(memcmp(input, dec, n) == 0);

        // URL round-trip (without padding)
        elen = xylem_base64_encode_url(input, n, enc, sizeof(enc), false);
        ASSERT(elen >= 0);
        dlen = xylem_base64_decode_url(enc, elen, dec, sizeof(dec), false);
        ASSERT(dlen == n);
        ASSERT(memcmp(input, dec, n) == 0);
    }
}

/**
 * Test extreme byte values: 0x00, 0xFF, 0x80, 0x7F.
 * Ensures bit manipulation handles sign and masking correctly.
 */
static void test_extreme_bytes(void) {
    uint8_t input[] = {0x00, 0xFF, 0x80, 0x7F};
    uint8_t enc[32], dec[16];

    int elen = xylem_base64_encode_std(input, 4, enc, sizeof(enc));
    ASSERT(elen == 8); // 4 bytes ¡ú ceil(4/3)*4 = 8

    int dlen = xylem_base64_decode_std(enc, elen, dec, sizeof(dec));
    ASSERT(dlen == 4);
    ASSERT(memcmp(input, dec, 4) == 0);
}

/**
 * Test that the decoder rejects Base64 strings with non-zero padding bits.
 *
 * In strict RFC 4648 compliance, when padding ('==') indicates only 1 original
 * byte, the unused low 4 bits of the second Base64 character MUST be zero.
 *
 * - "QQ==": 'Q' = 16 (0b010000) -> low 4 bits = 0000 -> valid
 * - "QR==": 'R' = 17 (0b010001) -> low 4 bits = 0001 != 0 -> invalid
 *
 * A compliant Base64 decoder must reject inputs with non-zero padding bits,
 * even if the decoded data appears plausible.
 */
static void test_reject_nonzero_padding_bits(void) {
    uint8_t buf[4];

    int len =
        xylem_base64_decode_std((const uint8_t*)"QQ==", 4, buf, sizeof(buf));
    ASSERT(len == 1);
    ASSERT(buf[0] == 0x41);

    len = xylem_base64_decode_std((const uint8_t*)"QR==", 4, buf, sizeof(buf));
    ASSERT(len == -1);
}

int main(void) {
    test_empty_input();
    test_one_byte();
    test_two_bytes();
    test_three_bytes();
    test_multi_block();
    test_illegal_characters();
    test_malformed_padding();
    test_non_multiple_of_4();
    test_insufficient_buffer();
    test_round_trip();
    test_extreme_bytes();
    test_reject_nonzero_padding_bits();
	return 0;
}