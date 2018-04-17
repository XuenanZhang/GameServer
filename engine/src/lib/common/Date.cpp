#include "Date.h"
#include "Timestamp.h"

using namespace bling;

Date::Date()
{
    _seconds = Timestamp::now().getSeconds();
    localtime_r(&_seconds, &_tm);
}

Date::Date(int secondsArg)
{
    _seconds = secondsArg;
    localtime_r(&_seconds, &_tm);
    
}
