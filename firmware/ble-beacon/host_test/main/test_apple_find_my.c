// Host-side logic unit tests for the Apple Find My advertisement encoder.
// Run on the IDF "linux" target with the bundled Unity -- no hardware.
//
// Byte layout asserted here matches OpenHaystack / Macless-Haystack firmware
// (set_addr_from_key / set_payload_from_key).

#include <string.h>

#include "unity.h"
#include "apple_find_my.h"

void setUp(void) {}
void tearDown(void) {}

// Deterministic key: key[i] = i, for i in 0..27.
static void fill_ramp_key(uint8_t key[APPLE_FIND_MY_KEY_LEN])
{
    for (int i = 0; i < APPLE_FIND_MY_KEY_LEN; i++) {
        key[i] = (uint8_t)i;
    }
}

static void test_address_from_ramp_key(void)
{
    uint8_t key[APPLE_FIND_MY_KEY_LEN];
    fill_ramp_key(key);

    uint8_t addr[APPLE_FIND_MY_ADDR_LEN];
    apple_find_my_set_address(addr, key);

    const uint8_t expected[APPLE_FIND_MY_ADDR_LEN] = {0xC0, 0x01, 0x02, 0x03, 0x04, 0x05};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, addr, APPLE_FIND_MY_ADDR_LEN);
}

static void test_address_top_bits_always_set(void)
{
    uint8_t key[APPLE_FIND_MY_KEY_LEN] = {0};
    uint8_t addr[APPLE_FIND_MY_ADDR_LEN];

    key[0] = 0x00;
    apple_find_my_set_address(addr, key);
    TEST_ASSERT_EQUAL_HEX8(0xC0, addr[0]);  // 0x00 | 0xC0

    key[0] = 0x3F;
    apple_find_my_set_address(addr, key);
    TEST_ASSERT_EQUAL_HEX8(0xFF, addr[0]);  // 0x3F | 0xC0

    key[0] = 0xFF;
    apple_find_my_set_address(addr, key);
    TEST_ASSERT_EQUAL_HEX8(0xFF, addr[0]);  // already set
}

static void test_adv_full_layout_for_ramp_key(void)
{
    uint8_t key[APPLE_FIND_MY_KEY_LEN];
    fill_ramp_key(key);

    uint8_t adv[APPLE_FIND_MY_ADV_LEN];
    apple_find_my_build_adv(adv, key);

    const uint8_t expected[APPLE_FIND_MY_ADV_LEN] = {
        0x1E, 0xFF, 0x4C, 0x00, 0x12, 0x19, 0x00,  // header
        6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,    // key[6..27]
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
        0x00,                                       // key[0] >> 6  (0x00 >> 6)
        0x00,                                       // hint
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, adv, APPLE_FIND_MY_ADV_LEN);
}

static void test_adv_high_bits_byte(void)
{
    uint8_t key[APPLE_FIND_MY_KEY_LEN] = {0};
    uint8_t adv[APPLE_FIND_MY_ADV_LEN];

    key[0] = 0xFF;  // 0xFF >> 6 == 0x03
    apple_find_my_build_adv(adv, key);
    TEST_ASSERT_EQUAL_HEX8(0x03, adv[29]);

    key[0] = 0x40;  // 0x40 >> 6 == 0x01
    apple_find_my_build_adv(adv, key);
    TEST_ASSERT_EQUAL_HEX8(0x01, adv[29]);
}

static void test_adv_header_is_constant(void)
{
    uint8_t key[APPLE_FIND_MY_KEY_LEN];
    fill_ramp_key(key);

    uint8_t adv[APPLE_FIND_MY_ADV_LEN];
    apple_find_my_build_adv(adv, key);

    const uint8_t header[7] = {0x1E, 0xFF, 0x4C, 0x00, 0x12, 0x19, 0x00};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(header, adv, 7);
    TEST_ASSERT_EQUAL_HEX8(0x00, adv[30]);  // hint always 0
}

void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_address_from_ramp_key);
    RUN_TEST(test_address_top_bits_always_set);
    RUN_TEST(test_adv_full_layout_for_ramp_key);
    RUN_TEST(test_adv_high_bits_byte);
    RUN_TEST(test_adv_header_is_constant);
    UNITY_END();
}
