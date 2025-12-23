#include "xylem.h"
#include "assert.h"

static void test_varint_basic_encode_decode(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 0;
    ASSERT(xylem_varint_encode(0, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 1);
    ASSERT(buffer[0] == 0);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 1);
    ASSERT(value == 0);
}

static void test_varint_small_values(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 0;
    ASSERT(xylem_varint_encode(1, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 1);
    ASSERT(buffer[0] == 1);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 1);
    ASSERT(value == 1);

    pos = 0;
    ASSERT(xylem_varint_encode(127, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 1);
    ASSERT(buffer[0] == 127);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 1);
    ASSERT(value == 127);
}

static void test_varint_two_byte_encoding(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 0;
    ASSERT(xylem_varint_encode(128, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 2);
    ASSERT(buffer[0] == 0x80);
    ASSERT(buffer[1] == 0x01);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 2);
    ASSERT(value == 128);

    pos = 0;
    ASSERT(xylem_varint_encode(16383, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 2);
    ASSERT(buffer[0] == 0xFF);
    ASSERT(buffer[1] == 0x7F);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 2);
    ASSERT(value == 16383);
}

static void test_varint_large_values(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 0;
    ASSERT(xylem_varint_encode(16384, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 3);
    ASSERT(buffer[0] == 0x80);
    ASSERT(buffer[1] == 0x80);
    ASSERT(buffer[2] == 0x01);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 3);
    ASSERT(value == 16384);

    pos = 0;
    ASSERT(xylem_varint_encode(2097151, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 3);
    ASSERT(buffer[0] == 0xFF);
    ASSERT(buffer[1] == 0xFF);
    ASSERT(buffer[2] == 0x7F);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 3);
    ASSERT(value == 2097151);

    pos = 0;
    ASSERT(xylem_varint_encode(2097152, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 4);
    ASSERT(buffer[0] == 0x80);
    ASSERT(buffer[1] == 0x80);
    ASSERT(buffer[2] == 0x80);
    ASSERT(buffer[3] == 0x01);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 4);
    ASSERT(value == 2097152);
}

static void test_varint_max_uint64(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 0;
    ASSERT(xylem_varint_encode(UINT64_MAX, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 10);
    ASSERT(buffer[0] == 0xFF);
    ASSERT(buffer[1] == 0xFF);
    ASSERT(buffer[2] == 0xFF);
    ASSERT(buffer[3] == 0xFF);
    ASSERT(buffer[4] == 0xFF);
    ASSERT(buffer[5] == 0xFF);
    ASSERT(buffer[6] == 0xFF);
    ASSERT(buffer[7] == 0xFF);
    ASSERT(buffer[8] == 0xFF);
    ASSERT(buffer[9] == 0x01);

    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(pos == 10);
    ASSERT(value == UINT64_MAX);
}

static void test_varint_encode_buffer_too_small(void) {
    uint8_t buffer[5];
    size_t  pos;

    pos = 0;
    ASSERT(xylem_varint_encode(1000000, buffer, 3, &pos));
    ASSERT(pos == 3);

    pos = 0;
    ASSERT(xylem_varint_encode(1000, buffer, 3, &pos));
    ASSERT(pos == 2);

    pos = 0;
    ASSERT(!xylem_varint_encode(1000000, buffer, 2, &pos));
    ASSERT(pos == 0);

    pos = 0;
    ASSERT(xylem_varint_encode(127, buffer, 1, &pos));
    ASSERT(pos == 1);

    pos = 0;
    ASSERT(!xylem_varint_encode(128, buffer, 1, &pos));
    ASSERT(pos == 0);
}

static void test_varint_decode_buffer_too_small(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    memset(buffer, 0x80, sizeof(buffer));
    buffer[0] = 0x80;
    buffer[1] = 0x80;

    pos = 0;
    ASSERT(!xylem_varint_decode(buffer, 1, &pos, &value));

    pos = 0;
    buffer[1] = 0x01;
    ASSERT(!xylem_varint_decode(buffer, 1, &pos, &value));

    pos = 0;
    buffer[0] = 0xFF;
    buffer[1] = 0x80;
    ASSERT(!xylem_varint_decode(buffer, 2, &pos, &value));
}

static void test_varint_encode_invalid_position(void) {
    uint8_t buffer[16];
    size_t  pos;

    pos = 20;
    ASSERT(!xylem_varint_encode(1, buffer, 10, &pos));

    pos = 0;
    ASSERT(xylem_varint_encode(1, buffer, 10, &pos));
    ASSERT(pos == 1);

    pos = 10;
    ASSERT(!xylem_varint_encode(1, buffer, 10, &pos));

    pos = 9;
    ASSERT(xylem_varint_encode(1, buffer, 10, &pos));
    ASSERT(pos == 10);
}

static void test_varint_decode_invalid_position(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    pos = 20;
    ASSERT(!xylem_varint_decode(buffer, 10, &pos, &value));

    pos = 10;
    ASSERT(!xylem_varint_decode(buffer, 10, &pos, &value));

    pos = 9;
    buffer[9] = 0;
    ASSERT(xylem_varint_decode(buffer, 10, &pos, &value));
    ASSERT(pos == 10);
}

static void test_varint_decode_incomplete_sequence(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    buffer[0] = 0x80;
    buffer[1] = 0x80;
    buffer[2] = 0x80;
    buffer[3] = 0x80;
    buffer[4] = 0x80;
    buffer[5] = 0x80;
    buffer[6] = 0x80;
    buffer[7] = 0x80;
    buffer[8] = 0x80;

    pos = 0;
    ASSERT(!xylem_varint_decode(buffer, 9, &pos, &value));
}

static void test_varint_decode_too_many_bytes(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t value;

    memset(buffer, 0x80, 11);
    buffer[10] = 0x00;

    pos = 0;
    ASSERT(!xylem_varint_decode(buffer, 11, &pos, &value));

    memset(buffer, 0x80, 10);
    buffer[9] = 0x7F;

    pos = 0;
    ASSERT(!xylem_varint_decode(buffer, 10, &pos, &value));
}

static void test_varint_encode_null_pointers(void) {
    uint8_t buffer[16];
    size_t  pos;

    pos = 0;
    ASSERT(!xylem_varint_encode(1, NULL, 10, &pos));

    pos = 0;
    ASSERT(!xylem_varint_encode(1, buffer, 0, &pos));

    pos = 0;
    ASSERT(xylem_varint_encode(1, buffer, 10, NULL));

    pos = 0;
    ASSERT(xylem_varint_encode(1, buffer, 10, &pos));
}

//static void test_varint_decode_null_pointers(void) {
//    uint8_t  buffer[16];
//    size_t   pos;
//    uint64_t value;
//
//    pos = 0;
//    ASSERT(!xylem_varint_decode(NULL, 10, &pos, &value));
//
//    pos = 0;
//    ASSERT(!xylem_varint_decode(buffer, 10, NULL, &value));
//
//    pos = 0;
//    ASSERT(xylem_varint_decode(buffer, 10, &pos, NULL));
//
//    pos = 0;
//    buffer[0] = 1;
//    ASSERT(xylem_varint_decode(buffer, 10, &pos, &value));
//    ASSERT(pos == 1);
//    ASSERT(value == 1);
//}

static void test_varint_roundtrip_random_values(void) {
    uint8_t  buffer[16];
    size_t   pos;
    uint64_t original;
    uint64_t decoded;

    uint64_t test_values[] = {
        0,
        1,
        2,
        127,
        128,
        255,
        256,
        1000,
        10000,
        65535,
        65536,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000ULL,
        100000000000ULL,
        1000000000000ULL,
        0xFFFFFFFFULL,
        0x7FFFFFFFFFFFFFFFULL,
        UINT64_MAX - 1};

    for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
        original = test_values[i];

        pos = 0;
        ASSERT(xylem_varint_encode(original, buffer, sizeof(buffer), &pos));

        size_t encoded_size = pos;
        pos = 0;
        ASSERT(xylem_varint_decode(buffer, encoded_size, &pos, &decoded));
        ASSERT(original == decoded);
        ASSERT(pos == encoded_size);
    }
}

static void test_varint_encode_without_position(void) {
    uint8_t buffer[16];

    ASSERT(xylem_varint_encode(42, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 42);

    ASSERT(xylem_varint_encode(128, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 0x80);
    ASSERT(buffer[1] == 0x01);
}

static void test_varint_encode_position_preserved_on_failure(void) {
    uint8_t buffer[2];
    size_t  pos;

    pos = 0;
    ASSERT(!xylem_varint_encode(100000, buffer, 2, &pos));
    ASSERT(pos == 0);

    buffer[0] = 0xFF;
    buffer[1] = 0x7F;
    pos = 0;
    ASSERT(xylem_varint_decode(buffer, 2, &pos, NULL));
    ASSERT(pos == 2);
}

static void test_varint_streaming_encode_multiple(void) {
    uint8_t  buffer[64];
    size_t   pos = 0;
    uint64_t values[] = {0, 1, 127, 128, 16383, 16384, UINT64_MAX};
    size_t   num_values = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < num_values; i++) {
        ASSERT(xylem_varint_encode(values[i], buffer, sizeof(buffer), &pos));
    }

    ASSERT(pos > 0);
}

static void test_varint_streaming_decode_multiple(void) {
    uint8_t  buffer[64];
    size_t   encode_pos = 0;
    uint64_t values[] = {0, 1, 127, 128, 16383, 16384, UINT64_MAX};
    size_t   num_values = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < num_values; i++) {
        ASSERT(xylem_varint_encode(
            values[i], buffer, sizeof(buffer), &encode_pos));
    }

    size_t decode_pos = 0;
    for (size_t i = 0; i < num_values; i++) {
        uint64_t decoded_value;
        ASSERT(xylem_varint_decode(
            buffer, encode_pos, &decode_pos, &decoded_value));
        ASSERT(decoded_value == values[i]);
    }

    ASSERT(decode_pos == encode_pos);
}

static void test_varint_streaming_partial_decode(void) {
    uint8_t  buffer[64];
    size_t   encode_pos = 0;
    uint64_t values[] = {42, 1000, 50000};

    for (size_t i = 0; i < 3; i++) {
        ASSERT(xylem_varint_encode(
            values[i], buffer, sizeof(buffer), &encode_pos));
    }

    size_t   decode_pos = 0;
    uint64_t decoded_value;

    ASSERT(
        xylem_varint_decode(buffer, encode_pos, &decode_pos, &decoded_value));
    ASSERT(decoded_value == 42);

    ASSERT(
        xylem_varint_decode(buffer, encode_pos, &decode_pos, &decoded_value));
    ASSERT(decoded_value == 1000);

    ASSERT(decode_pos < encode_pos);
}

static void test_varint_streaming_decode_beyond_buffer(void) {
    uint8_t buffer[16];
    size_t  encode_pos = 0;

    ASSERT(xylem_varint_encode(100, buffer, sizeof(buffer), &encode_pos));
    ASSERT(xylem_varint_encode(200, buffer, sizeof(buffer), &encode_pos));

    size_t   decode_pos = 0;
    uint64_t value;

    ASSERT(xylem_varint_decode(buffer, encode_pos, &decode_pos, &value));
    ASSERT(value == 100);

    ASSERT(xylem_varint_decode(buffer, encode_pos, &decode_pos, &value));
    ASSERT(value == 200);

    ASSERT(!xylem_varint_decode(buffer, encode_pos, &decode_pos, &value));
}

static void test_varint_streaming_interleaved_operations(void) {
    uint8_t buffer[32];
    size_t  pos = 0;

    ASSERT(xylem_varint_encode(10, buffer, sizeof(buffer), &pos));
    ASSERT(xylem_varint_encode(20, buffer, sizeof(buffer), &pos));

    size_t   read_pos = 0;
    uint64_t value;

    ASSERT(xylem_varint_decode(buffer, pos, &read_pos, &value));
    ASSERT(value == 10);

    ASSERT(xylem_varint_encode(30, buffer, sizeof(buffer), &pos));

    ASSERT(xylem_varint_decode(buffer, pos, &read_pos, &value));
    ASSERT(value == 20);

    ASSERT(xylem_varint_decode(buffer, pos, &read_pos, &value));
    ASSERT(value == 30);

    ASSERT(read_pos == pos);
}

static void test_varint_streaming_large_sequence(void) {
    uint8_t buffer[1024];
    size_t  encode_pos = 0;

    for (uint64_t i = 0; i < 100; i++) {
        ASSERT(
            xylem_varint_encode(i * 1000, buffer, sizeof(buffer), &encode_pos));
    }

    size_t decode_pos = 0;
    for (uint64_t i = 0; i < 100; i++) {
        uint64_t decoded_value;
        ASSERT(xylem_varint_decode(
            buffer, encode_pos, &decode_pos, &decoded_value));
        ASSERT(decoded_value == i * 1000);
    }

    ASSERT(decode_pos == encode_pos);
}

static void test_varint_streaming_decode_with_offset(void) {
    uint8_t buffer[64];
    size_t  pos = 0;

    for (int i = 0; i < 5; i++) {
        ASSERT(xylem_varint_encode(i, buffer, sizeof(buffer), &pos));
    }

    uint64_t value;
    size_t   decode_pos = 2;

    if (decode_pos < pos) {
        size_t temp_pos = decode_pos;
        ASSERT(xylem_varint_decode(buffer, pos, &temp_pos, &value));
        ASSERT(value == 2);
    }
}

static void test_varint_streaming_mixed_size_values(void) {
    uint8_t buffer[128];
    size_t  encode_pos = 0;

    uint64_t values[] = {
        0,
        127,
        128,
        255,
        256,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000ULL,
        100000000000ULL};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        ASSERT(xylem_varint_encode(
            values[i], buffer, sizeof(buffer), &encode_pos));
    }

    size_t decode_pos = 0;
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        uint64_t decoded_value;
        ASSERT(xylem_varint_decode(
            buffer, encode_pos, &decode_pos, &decoded_value));
        ASSERT(decoded_value == values[i]);
    }
}

static void test_varint_streaming_buffer_boundary(void) {
    uint8_t buffer[10];
    size_t  pos = 0;

    ASSERT(xylem_varint_encode(100, buffer, sizeof(buffer), &pos));
    ASSERT(pos == 1);

    size_t remaining = sizeof(buffer) - pos;

    uint64_t large_value = 0xFFFFFFFF;
    bool     encode_result =
        xylem_varint_encode(large_value, buffer, sizeof(buffer), &pos);

    if (encode_result) {
        size_t   decode_pos = 0;
        uint64_t value1, value2;

        ASSERT(xylem_varint_decode(buffer, pos, &decode_pos, &value1));
        ASSERT(value1 == 100);

        ASSERT(xylem_varint_decode(buffer, pos, &decode_pos, &value2));
        ASSERT(value2 == large_value);
    }
}

static void test_varint_streaming_resume_after_error(void) {
    uint8_t buffer[20];
    size_t  pos = 0;

    ASSERT(xylem_varint_encode(100, buffer, sizeof(buffer), &pos));

    uint8_t* bad_buffer = NULL;
    size_t   bad_pos = 0;
    uint64_t dummy;

    ASSERT(!xylem_varint_decode(bad_buffer, 10, &bad_pos, &dummy));

    size_t   decode_pos = 0;
    uint64_t value;
    ASSERT(xylem_varint_decode(buffer, pos, &decode_pos, &value));
    ASSERT(value == 100);
}

static void test_varint_size_function(void) {
    ASSERT(xylem_varint_compute(0) == 1);
    ASSERT(xylem_varint_compute(1) == 1);
    ASSERT(xylem_varint_compute(127) == 1);
    ASSERT(xylem_varint_compute(128) == 2);
    ASSERT(xylem_varint_compute(16383) == 2);
    ASSERT(xylem_varint_compute(16384) == 3);
    ASSERT(xylem_varint_compute(2097151) == 3);
    ASSERT(xylem_varint_compute(2097152) == 4);
    ASSERT(xylem_varint_compute(268435455) == 4);
    ASSERT(xylem_varint_compute(268435456) == 5);
    ASSERT(xylem_varint_compute(UINT64_MAX) == 10);
    ASSERT(xylem_varint_compute(100) == 1);
    ASSERT(xylem_varint_compute(1000) == 2);
    ASSERT(xylem_varint_compute(10000) == 2);
    ASSERT(xylem_varint_compute(100000) == 3);
    ASSERT(xylem_varint_compute(1000000) == 3);
    ASSERT(xylem_varint_compute(10000000) == 4);
    ASSERT(xylem_varint_compute(100000000) == 4);
    ASSERT(xylem_varint_compute(1000000000) == 5);
}

static void test_varint_encode_with_null_pos(void) {
    uint8_t buffer[16];
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(0, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 0);
    ASSERT(buffer[1] == 0xFF);
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(1, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 1);
    ASSERT(buffer[1] == 0xFF);
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(127, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 127);
    ASSERT(buffer[1] == 0xFF);
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(128, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 0x80);
    ASSERT(buffer[1] == 0x01);
    ASSERT(buffer[2] == 0xFF);
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(UINT64_MAX, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 0xFF);
    ASSERT(buffer[1] == 0xFF);
    ASSERT(buffer[2] == 0xFF);
    ASSERT(buffer[3] == 0xFF);
    ASSERT(buffer[4] == 0xFF);
    ASSERT(buffer[5] == 0xFF);
    ASSERT(buffer[6] == 0xFF);
    ASSERT(buffer[7] == 0xFF);
    ASSERT(buffer[8] == 0xFF);
    ASSERT(buffer[9] == 0x01);
    ASSERT(buffer[10] == 0xFF);
}

static void test_varint_encode_null_pos_buffer_too_small(void) {
    uint8_t buffer[3];
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(xylem_varint_encode(1000, buffer, 2, NULL));
    ASSERT(buffer[0] == 0xE8);
    ASSERT(buffer[1] == 0x07);
    ASSERT(buffer[2] == 0xFF);
    memset(buffer, 0xFF, sizeof(buffer));
    ASSERT(!xylem_varint_encode(100000, buffer, 2, NULL));
}

static void test_varint_encode_null_pos_zero_buffer(void) {
    uint8_t buffer[1];
    ASSERT(!xylem_varint_encode(0, buffer, 0, NULL));
    ASSERT(!xylem_varint_encode(1, buffer, 0, NULL));
}

static void test_varint_encode_null_pos_invalid_buffer(void) {
    ASSERT(!xylem_varint_encode(0, NULL, 10, NULL));
    ASSERT(!xylem_varint_encode(0, NULL, 0, NULL));
}

static void test_varint_encode_null_pos_with_size_function(void) {
    uint8_t  buffer[16];
    uint64_t test_values[] = {
        0,
        1,
        127,
        128,
        16383,
        16384,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000ULL,
        100000000000ULL,
        UINT64_MAX};
    for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
        uint64_t value = test_values[i];
        size_t   expected_size = xylem_varint_compute(value);
        ASSERT(expected_size <= sizeof(buffer));

        bool encode_ok =
            xylem_varint_encode(value, buffer, sizeof(buffer), NULL);
        ASSERT(encode_ok);

        size_t   decode_pos = 0;
        uint64_t decoded_value;
        bool     decode_ok = xylem_varint_decode(
            buffer, expected_size, &decode_pos, &decoded_value);
        ASSERT(decode_ok);
        ASSERT(decoded_value == value);
        ASSERT(decode_pos == expected_size);

        size_t pos2 = 0;
        memset(buffer, 0, sizeof(buffer));
        ASSERT(xylem_varint_encode(value, buffer, sizeof(buffer), &pos2));
        ASSERT(pos2 == expected_size);
    }
}

static void test_varint_encode_null_pos_then_decode(void) {
    uint8_t buffer[32];
    ASSERT(xylem_varint_encode(123456, buffer, sizeof(buffer), NULL));
    size_t   pos = 0;
    uint64_t value;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(value == 123456);
    ASSERT(xylem_varint_encode(789012, buffer, sizeof(buffer), NULL));
    pos = 0;
    ASSERT(xylem_varint_decode(buffer, sizeof(buffer), &pos, &value));
    ASSERT(value == 789012);
}

static void test_varint_encode_null_pos_does_not_modify_beyond_encoded(void) {
    uint8_t buffer[16];
    memset(buffer, 0xAA, sizeof(buffer));
    ASSERT(xylem_varint_encode(42, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 42);
    for (size_t i = 1; i < sizeof(buffer); i++) {
        ASSERT(buffer[i] == 0xAA);
    }
    memset(buffer, 0xBB, sizeof(buffer));
    ASSERT(xylem_varint_encode(100000, buffer, sizeof(buffer), NULL));
    ASSERT(buffer[0] == 0xA0);
    ASSERT(buffer[1] == 0x8D);
    ASSERT(buffer[2] == 0x06);
    for (size_t i = 3; i < sizeof(buffer); i++) {
        ASSERT(buffer[i] == 0xBB);
    }
}

int main(void) {
    test_varint_basic_encode_decode();
    test_varint_small_values();
    test_varint_two_byte_encoding();
    test_varint_large_values();
    test_varint_max_uint64();
    test_varint_encode_buffer_too_small();
    test_varint_decode_buffer_too_small();
    test_varint_encode_invalid_position();
    test_varint_decode_invalid_position();
    test_varint_decode_incomplete_sequence();
    test_varint_decode_too_many_bytes();
    test_varint_encode_null_pointers();
    //test_varint_decode_null_pointers();
    test_varint_roundtrip_random_values();
    test_varint_encode_without_position();
    test_varint_encode_position_preserved_on_failure();
    test_varint_streaming_encode_multiple();
    test_varint_streaming_decode_multiple();
    test_varint_streaming_partial_decode();
    test_varint_streaming_decode_beyond_buffer();
    test_varint_streaming_interleaved_operations();
    test_varint_streaming_large_sequence();
    test_varint_streaming_decode_with_offset();
    test_varint_streaming_mixed_size_values();
    test_varint_streaming_buffer_boundary();
    test_varint_streaming_resume_after_error();
    test_varint_size_function();
    test_varint_encode_with_null_pos();
    test_varint_encode_null_pos_buffer_too_small();
    test_varint_encode_null_pos_zero_buffer();
    test_varint_encode_null_pos_invalid_buffer();
    test_varint_encode_null_pos_with_size_function();
    test_varint_encode_null_pos_then_decode();
    test_varint_encode_null_pos_does_not_modify_beyond_encoded();
    return 0;
}