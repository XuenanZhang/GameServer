#include "Date.h"
#include "Timestamp.h"
// #include <ctime>

using namespace bling;

// const int secondsPerDay = 24*60*60;

Date::Date()
{
    now();
}

Date::Date(int secondsUtc)
{
    _seconds = secondsUtc;
    localtime_r(&_seconds, &_tm);
    
}

void Date::now()
{
    _seconds = Timestamp::now().getSeconds();
    localtime_r(&_seconds, &_tm);
}

Timestamp Date::getZero()
{
    _tm.tm_sec = 0;
    _tm.tm_min = 0;
    _tm.tm_hour = 0;
    int sec = static_cast<int>(mktime(&_tm));
    return Timestamp(sec * Timestamp::kMicroSecondPerSecond);
}
