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

int xylem_base64_encode(
    const uint8_t* src, int srclen, uint8_t* dst, int dstlen) {
    if (srclen < 0 || dstlen < 0 || srclen > 0 && src == NULL) {
        return -1;
    }
    if (dst == NULL) {
        return -1;
    }
    if (dstlen < ((srclen + 2) / 3) * 4) {
        return -1;
    }
    static const char b64[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i, j;
    for (i = 0, j = 0; i < srclen; i += 3, j += 4) {
        uint32_t n = ((uint32_t)src[i]) << 16;
        if (i + 1 < srclen) {
            n |= ((uint32_t)src[i + 1]) << 8;
        }
        if (i + 2 < srclen) {
            n |= ((uint32_t)src[i + 2]);
        }
        dst[j] = b64[(n >> 18) & 63];
        dst[j + 1] = b64[(n >> 12) & 63];
        dst[j + 2] = (i + 1 < srclen) ? b64[(n >> 6) & 63] : '=';
        dst[j + 3] = (i + 2 < srclen) ? b64[n & 63] : '=';
    }
    return j;
}

int xylem_base64_decode(
    const uint8_t* src, int srclen, uint8_t* dst, int dstlen) {
    if (srclen < 0 || dstlen < 0 || src == NULL || dst == NULL) {
        return -1;
    }
    if (srclen % 4 != 0) {
        return -1;
    }
    if (dstlen < (srclen / 4) * 3) {
        return -1;
    }
    static const uint8_t b64[256] = {
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,
        7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64};

    int    out = 0;
    size_t i;

    for (i = 0; i < srclen; i += 4) {
        uint8_t c0 = src[i];
        uint8_t c1 = src[i + 1];
        uint8_t c2 = src[i + 2];
        uint8_t c3 = src[i + 3];

        if (c2 == '=' && c3 == '=') {
            if (b64[c0] == 64 || b64[c1] == 64) {
                return -1;
            }
            uint32_t n = (b64[c0] << 18) | (b64[c1] << 12);
            dst[out++] = (n >> 16) & 0xFF;
        } else if (c3 == '=') {
            if (b64[c0] == 64 || b64[c1] == 64 || b64[c2] == 64) {
                return -1;
            }
            uint32_t n = (b64[c0] << 18) | (b64[c1] << 12) | (b64[c2] << 6);
            dst[out++] = (n >> 16) & 0xFF;
            dst[out++] = (n >> 8) & 0xFF;
        } else {
            if (b64[c0] == 64 || b64[c1] == 64 || b64[c2] == 64 ||
                b64[c3] == 64) {
                return -1;
            }
            uint32_t n =
                (b64[c0] << 18) | (b64[c1] << 12) | (b64[c2] << 6) | b64[c3];
            dst[out++] = (n >> 16) & 0xFF;
            dst[out++] = (n >> 8) & 0xFF;
            dst[out++] = n & 0xFF;
        }
    }
    return out;
}