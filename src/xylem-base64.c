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

static const char b64_enc_std[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char b64_enc_url[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static const uint8_t b64_dec_std[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64};

static const uint8_t b64_dec_url[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    63, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64};

static int _base64_encode_with_padding(
    const uint8_t* src, int slen, uint8_t* dst, int dlen, const char* table) {
    if (slen < 0 || dlen < 0 || !src || !dst) {
        return -1;
    }
    /**
     * Base64 Encoding Rule:
     *   - Every 3 input bytes are encoded into 4 output characters.
     *   - Padding with '=' is added if the input length is not a multiple of 3.
     *
     * Output Length Formula:
     *   The number of bytes required for the Base64-encoded output is:
     *       ((slen + 2) / 3) * 4
     *   This expression computes ceil(slen / 3) * 4 using integer arithmetic.
     */
    if (dlen < ((slen + 2) / 3) * 4) {
        return -1;
    }
    // Encode full 3-byte groups (no padding needed)
    int i, j;
    for (i = 0, j = 0; i < (slen / 3) * 3; i += 3, j += 4) {
        uint32_t n = ((uint32_t)src[i] << 16) | ((uint32_t)src[i + 1] << 8) |
                     ((uint32_t)src[i + 2]);

        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];
        dst[j + 2] = table[(n >> 6) & 0x3F];
        dst[j + 3] = table[n & 0x3F];
    }
    // Handle remaining bytes (0, 1, or 2)
    switch (slen % 3) {
    case 1: {
        uint32_t n = (uint32_t)src[slen - 1] << 16;
        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];

        dst[j + 2] = '=';
        dst[j + 3] = '=';
        j += 4;
        break;
    }
    case 2: {
        uint32_t n =
            ((uint32_t)src[slen - 2] << 16) | ((uint32_t)src[slen - 1] << 8);
        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];
        dst[j + 2] = table[(n >> 6) & 0x3F];

        dst[j + 3] = '=';
        j += 4;
        break;
    }
    }
    return j;
}

static int _base64_encode_without_padding(
    const uint8_t* src, int slen, uint8_t* dst, int dlen, const char* table) {
    if (slen < 0 || dlen < 0 || !src || !dst) {
        return -1;
    }
    if (dlen < ((slen + 2) / 3) * 4) {
        return -1;
    }
    int i, j;
    for (i = 0, j = 0; i < (slen / 3) * 3; i += 3, j += 4) {
        uint32_t n = ((uint32_t)src[i] << 16) | ((uint32_t)src[i + 1] << 8) |
                     ((uint32_t)src[i + 2]);

        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];
        dst[j + 2] = table[(n >> 6) & 0x3F];
        dst[j + 3] = table[n & 0x3F];
    }
    switch (slen % 3) {
    case 1: {
        uint32_t n = (uint32_t)src[slen - 1] << 16;
        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];

        j += 2;
        break;
    }
    case 2: {
        uint32_t n =
            ((uint32_t)src[slen - 2] << 16) | ((uint32_t)src[slen - 1] << 8);
        dst[j] = table[(n >> 18) & 0x3F];
        dst[j + 1] = table[(n >> 12) & 0x3F];
        dst[j + 2] = table[(n >> 6) & 0x3F];

        j += 3;
        break;
    }
    }
    return j;
}

static int _base64_decode_with_padding(
    const uint8_t* src,
    int            slen,
    uint8_t*       dst,
    int            dlen,
    const uint8_t* table) {
    if (slen < 0 || dlen < 0 || !src || !dst) {
        return -1;
    }
    if (slen % 4 != 0) {
        return -1;
    }
    if (dlen < (slen / 4) * 3) {
        return -1;
    }
    // Decode full 4-chars groups (no padding needed)
    int i, j = 0;
    for (i = 0; i < ((slen / 4) - 1) * 4; i += 4) {
        uint8_t c0 = src[i];
        uint8_t c1 = src[i + 1];
        uint8_t c2 = src[i + 2];
        uint8_t c3 = src[i + 3];

        if (table[c0] == 64 || table[c1] == 64 || table[c2] == 64 ||
            table[c3] == 64) {
            return -1;
        }
        uint32_t n = ((uint32_t)table[c0] << 18) | ((uint32_t)table[c1] << 12) |
                     ((uint32_t)table[c2] << 6) | (uint32_t)table[c3];

        dst[j++] = (n >> 16) & 0xFF;
        dst[j++] = (n >> 8) & 0xFF;
        dst[j++] = n & 0xFF;
    }
    /**
     * Handle the last 4-character group, which may contain padding ('=').
     * This group determines the actual number of output bytes (3, 2, or 1)
     * based on the presence and position of '=' characters.
     */
    if (slen > 0) {
        uint8_t c0 = src[slen - 4];
        uint8_t c1 = src[slen - 3];
        uint8_t c2 = src[slen - 2];
        uint8_t c3 = src[slen - 1];

        if (table[c0] == 64 || table[c1] == 64) {
            return -1;
        }

        int pad = (c2 == '=') + (c3 == '=');
        switch (pad) {
        case 0:
            if (table[c0] == 64 || table[c1] == 64 || table[c2] == 64 ||
                table[c3] == 64) {
                return -1;
            }
            {
                uint32_t n = ((uint32_t)table[c0] << 18) |
                             ((uint32_t)table[c1] << 12) |
                             ((uint32_t)table[c2] << 6) | (uint32_t)table[c3];
                dst[j++] = (n >> 16) & 0xFF;
                dst[j++] = (n >> 8) & 0xFF;
                dst[j++] = n & 0xFF;
            }
            break;
        case 1:
            if (c3 != '=' || c2 == '=' || table[c2] == 64) {
                return -1;
            }
            /**
             * In this case (e.g., "QQQ="), the encoded data represents exactly
             * 2 bytes of input. The third Base64 character (c2) contributes
             * only its top 4 bits to the output; the lower 2 bits are padding
             * and MUST be zero per RFC 4648 ¡ì3.5.
             *
             * Example:
             *   - "QQQ=": 'Q' = 16 = 0b010000 ¡ú low 2 bits = 00 ¡ú valid.
             *   - "QQR=": 'R' = 17 = 0b010001 ¡ú low 2 bits = 01 ¡Ù 0 ¡ú invalid.
             *
             * Non-zero padding bits indicate a non-canonical encoding, which is
             * rejected to ensure deterministic decoding and prevent security
             * issues (e.g., signature or token confusion).
             */
            if (table[c2] & 0x03) {
                return -1;
            }
            {
                uint32_t n = ((uint32_t)table[c0] << 18) |
                             ((uint32_t)table[c1] << 12) |
                             ((uint32_t)table[c2] << 6);

                dst[j++] = (n >> 16) & 0xFF;
                dst[j++] = (n >> 8) & 0xFF;
            }
            break;

        case 2:
            if (c2 != '=' || c3 != '=') {
                return -1;
            }
            /**
             * In this case (e.g., "QQ=="), the encoded data represents exactly
             * 1 byte of input. The second Base64 character contributes only its
             * top 2 bits to the output; the lower 4 bits are padding and MUST
             * be zero per RFC 4648 ¡ì3.5.
             *
             * Example:
             *   - "QQ==": 'Q' = 16 = 0b010000 ¡ú low 4 bits = 0000 ¡ú valid.
             *   - "QR==": 'R' = 17 = 0b010001 ¡ú low 4 bits = 0001 ¡Ù 0 ¡ú
             * invalid.
             *
             * Non-zero padding bits indicate a non-canonical encoding, which is
             * rejected to ensure deterministic decoding and prevent security
             * issues (e.g., signature confusion).
             */
            if (table[c1] & 0x0F) {
                return -1;
            }
            {
                uint32_t n =
                    ((uint32_t)table[c0] << 18) | ((uint32_t)table[c1] << 12);

                dst[j++] = (n >> 16) & 0xFF;
            }
            break;
        }
    }
    return j;
}

static int _base64_decode_without_padding(
    const uint8_t* src,
    int            slen,
    uint8_t*       dst,
    int            dlen,
    const uint8_t* table) {
    if (slen < 0 || dlen < 0 || !src || !dst) {
        return -1;
    }
    int full_groups = slen / 4;
    int remainder = slen % 4;
    int expected_out_len;

    switch (remainder) {
    case 0:
        expected_out_len = full_groups * 3;
        break;
    case 2:
        expected_out_len = full_groups * 3 + 1;
        break;
    case 3:
        expected_out_len = full_groups * 3 + 2;
        break;
    case 1:
        // Invalid: 1 trailing Base64 char cannot form a complete byte
        return -1;
    }
    if (dlen < expected_out_len) {
        return -1;
    }

    // Decode full 4-char groups
    int i, j = 0;
    for (i = 0; i < full_groups * 4; i += 4) {
        uint8_t c0 = src[i];
        uint8_t c1 = src[i + 1];
        uint8_t c2 = src[i + 2];
        uint8_t c3 = src[i + 3];

        if (table[c0] == 64 || table[c1] == 64 || table[c2] == 64 ||
            table[c3] == 64) {
            return -1;
        }
        uint32_t n = ((uint32_t)table[c0] << 18) | ((uint32_t)table[c1] << 12) |
                     ((uint32_t)table[c2] << 6) | (uint32_t)table[c3];

        dst[j++] = (n >> 16) & 0xFF;
        dst[j++] = (n >> 8) & 0xFF;
        dst[j++] = n & 0xFF;
    }
    // Handle trailing characters (remainder = 2 or 3)
    switch (remainder) {
    case 2:
        uint8_t c0 = src[i], c1 = src[i + 1];
        if (table[c0] == 64 || table[c1] == 64) {
            return -1;
        }
        // Only the top 8 bits of the 12-bit value are used
        {
            uint32_t n =
                ((uint32_t)table[c0] << 18) | ((uint32_t)table[c1] << 12);

            dst[j++] = (n >> 16) & 0xFF;
        }
        break;
    case 3:
        uint8_t c0 = src[i], c1 = src[i + 1], c2 = src[i + 2];
        if (table[c0] == 64 || table[c1] == 64 || table[c2] == 64) {
            return -1;
        }
        // Only the top 16 bits of the 18-bit value are used
        {
            uint32_t n = ((uint32_t)table[c0] << 18) |
                         ((uint32_t)table[c1] << 12) |
                         ((uint32_t)table[c2] << 6);

            dst[j++] = (n >> 16) & 0xFF;
            dst[j++] = (n >> 8) & 0xFF;
        }
        break;
    }
    return j;
}

int xylem_base64_encode_std(
    const uint8_t* src, int slen, uint8_t* dst, int dlen) {
    return _base64_encode_with_padding(src, slen, dst, dlen, b64_enc_std);
}

int xylem_base64_decode_std(
    const uint8_t* src, int slen, uint8_t* dst, int dlen) {
    return _base64_decode_with_padding(src, slen, dst, dlen, b64_dec_std);
}

int xylem_base64_encode_url(
    const uint8_t* src, int slen, uint8_t* dst, int dlen, bool padding) {
    if (padding) {
        return _base64_encode_with_padding(src, slen, dst, dlen, b64_enc_url);
    } else {
        return _base64_encode_without_padding(
            src, slen, dst, dlen, b64_enc_url);
    }
}

int xylem_base64_decode_url(
    const uint8_t* src, int slen, uint8_t* dst, int dlen, bool padding) {
    if (padding) {
        return _base64_decode_with_padding(src, slen, dst, dlen, b64_dec_url);
    } else {
        return _base64_decode_without_padding(
            src, slen, dst, dlen, b64_dec_url);
    }
}