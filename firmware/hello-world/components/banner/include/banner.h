#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Format a millisecond uptime as "Dd HH:MM:SS.mmm" into @p buf.
 *
 * Pure logic with no hardware or RTOS dependencies, so it is unit-tested on the
 * host (see ../../host_test). Behaves like snprintf: writes at most @p len bytes
 * (always NUL-terminated when @p len > 0) and returns the number of characters
 * that *would* have been written, so callers can detect truncation.
 *
 * @param buf     Destination buffer.
 * @param len     Size of @p buf in bytes.
 * @param millis  Uptime in milliseconds.
 * @return Number of characters (excluding NUL) the full string needs.
 */
int banner_format_uptime(char *buf, size_t len, uint64_t millis);

#ifdef __cplusplus
}
#endif
