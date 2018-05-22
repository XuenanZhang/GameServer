#include "net/Timer.h"

using namespace bling;
using namespace bling::net;

std::atomic<int64_t> Timer::_s_numCreated(0);

void Timer::restart(Timestamp now)
{
    if (_repeat)
    {
        _endTime = addTime(now, _interval);
    }
    else
    {
        _endTime = Timestamp::invalid(); 
    }
}
