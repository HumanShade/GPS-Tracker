// Host logic unit tests for the banner component (Phase 0).
// Portable: builds with any host C compiler via firmware/test/CMakeLists.txt.

#include <string.h>

#include "unity.h"
#include "banner.h"

void setUp(void) {}
void tearDown(void) {}

static void test_uptime_zero(void)
{
    char buf[32];
    int n = banner_format_uptime(buf, sizeof(buf), 0);
    TEST_ASSERT_EQUAL_STRING("0d 00:00:00.000", buf);
    TEST_ASSERT_EQUAL_INT(15, n);
}

static void test_uptime_seconds_and_millis(void)
{
    char buf[32];
    banner_format_uptime(buf, sizeof(buf), 1234);
    TEST_ASSERT_EQUAL_STRING("0d 00:00:01.234", buf);
}

static void test_uptime_hms_rollover(void)
{
    char buf[32];
    // 1h 02m 03.004s
    uint64_t ms = ((1ULL * 3600) + (2 * 60) + 3) * 1000 + 4;
    banner_format_uptime(buf, sizeof(buf), ms);
    TEST_ASSERT_EQUAL_STRING("0d 01:02:03.004", buf);
}

static void test_uptime_days(void)
{
    char buf[32];
    // 2 days, 3h 04m 05.006s
    uint64_t ms = ((2ULL * 86400) + (3 * 3600) + (4 * 60) + 5) * 1000 + 6;
    banner_format_uptime(buf, sizeof(buf), ms);
    TEST_ASSERT_EQUAL_STRING("2d 03:04:05.006", buf);
}

static void test_uptime_truncation_reports_full_length(void)
{
    char buf[8];  // too small for "0d 00:00:00.000"
    int n = banner_format_uptime(buf, sizeof(buf), 0);
    TEST_ASSERT_EQUAL_INT(15, n);          // snprintf semantics: full length
    TEST_ASSERT_EQUAL_INT(0, buf[7]);      // still NUL-terminated within buf
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_uptime_zero);
    RUN_TEST(test_uptime_seconds_and_millis);
    RUN_TEST(test_uptime_hms_rollover);
    RUN_TEST(test_uptime_days);
    RUN_TEST(test_uptime_truncation_reports_full_length);
    return UNITY_END();
}
