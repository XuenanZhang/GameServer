/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-08 13:44
 * Last modified : 2018-05-08 13:44
 * Filename      : Timer.h

 * Description   : 定时任务
 * *******************************************************/
#ifndef _BLING_TIMER_H_
#define _BLING_TIMER_H_

#include "common/noncopyable.h"
#include "common/Timestamp.h"
#include "net/Callbacks.h"

#include <atomic>
namespace bling
{

namespace net 
{

class Timer : bling::noncopyable
{
public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : _callback(cb),
          _endTime(when), 
          _interval(interval), 
          _repeat(interval > 0.0), 
          _sequence(_s_numCreated++)
    {
    }

    void run() const { _callback(); }

    Timestamp endTime() const { return _endTime; }

    bool repeat() const { return _repeat; }

    void restart(Timestamp now);

    int64_t sequence() { return _sequence; }

    static int64_t numCreated() { return _s_numCreated.load(); }

private:
    const TimerCallback _callback;
    Timestamp _endTime;
    const double _interval;
    const bool _repeat;
    const int64_t _sequence;

    static std::atomic<int64_t> _s_numCreated;
}; // class Timer

}; // ns net

}; //ns bling


#endif // _BLING_TIMER_H_
