#include "common/Timestamp.h"
#include "common/Date.h"
#include "common/Utils.h"

#include <sys/time.h>
#include <stdio.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

using namespace bling;


string Timestamp::toString()
{
    char buf[32] = {0};
    int64_t seconds = _microSeconds / kMicroSecondPerSecond;
    int64_t microSeconds = _microSeconds % kMicroSecondPerSecond;
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%" PRId64 "", seconds, microSeconds);

    return buf;
}


string Timestamp::toFormatString(bool showMicorSeconds)
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(_microSeconds / kMicroSecondPerSecond);
    Date date(seconds);
    localtime_r(NULL, NULL);

    if (showMicorSeconds)
    {
        int32_t microseconds = static_cast<int32_t>(_microSeconds % kMicroSecondPerSecond);
        snprintf(buf, sizeof(buf) - 1, "%4d-%02d-%02d %02d:%02d:%02d.%06d",
                date.year(), date.month(), date.day(), date.hour(), date.minutes(), date.seconds(), microseconds );
    }
    else
    {
        snprintf(buf, sizeof(buf) - 1, "%4d-%02d-%02d %02d:%02d:%02d",
                date.year(), date.month(), date.day(), date.hour(), date.minutes(), date.seconds());
    }

    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return Timestamp( tv.tv_sec * kMicroSecondPerSecond + tv.tv_usec);
}


double RandTimer::rand()
{
    return _howlong / 2 + utils::random(0, static_cast<int>(_howlong * 1000)) / 1000.f;   
}
