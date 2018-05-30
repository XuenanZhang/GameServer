/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-08 17:46
 * Last modified : 2018-05-08 17:46
 * Filename      : TimerQueue.h

 * Description   : timer队列管理器
 * *******************************************************/
#ifndef _BLING_TIMERQUEUE_H_
#define _BLING_TIMERQUEUE_H_

#include "common/noncopyable.h"
#include "common/Timestamp.h"
#include "net/Callbacks.h"
#include "net/Channel.h"

#include <set>

namespace bling
{

namespace net 
{

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : bling::noncopyable 
{
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerSet;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    void handleRead();

    /** 到期列表 **/
    std::vector<Entry> getExpired(Timestamp now);

    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

private:
    EventLoop* _loop;
    const int _timerfd;
    Channel _timerfdChannel;
    TimerSet _timers;

    ActiveTimerSet _activeTimers;
    ActiveTimerSet _cancelingTimers;
    bool _callingExpiredTimers;
    // std::set<Timer*> _cancelingTimers;

}; // class TimerQueue : bling::noncopyable

}; // ns net

}; //ns bling


#endif // _BLING_TIMERQUEUE_H_
