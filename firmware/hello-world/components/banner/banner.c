#include "banner.h"

#include <stdio.h>

int banner_format_uptime(char *buf, size_t len, uint64_t millis)
{
    uint64_t total_seconds = millis / 1000u;

    unsigned ms    = (unsigned)(millis % 1000u);
    unsigned secs  = (unsigned)(total_seconds % 60u);
    unsigned mins  = (unsigned)((total_seconds / 60u) % 60u);
    unsigned hours = (unsigned)((total_seconds / 3600u) % 24u);
    unsigned long long days = (unsigned long long)(total_seconds / 86400u);

    return snprintf(buf, len, "%llud %02u:%02u:%02u.%03u",
                    days, hours, mins, secs, ms);
}
