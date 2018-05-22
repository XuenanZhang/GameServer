#include "net/TimerQueue.h"
#include "log/Logger.h"
#include "net/TimerId.h"
#include "net/Timer.h"
#include "net/EventLoop.h"

#include <sys/timerfd.h>
#include <unistd.h>

namespace bling 
{

namespace net 
{

namespace detail 
{

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0)
    {
        LOG_SYSFATAL << "Failed in timerfd_create";
    }

    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.getMircoSeconds() - Timestamp::now().getMircoSeconds(); 

    if (microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondPerSecond) * 1000);

    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if (n != sizeof howmany)
    {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, Timestamp endTime)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);

    newValue.it_value = howMuchTimeFromNow(endTime);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_SYSERR << "timerfd_settiem() ret = " << ret;
    }
}

}; // ns detail

}; // ns net

}; // ns bling

using namespace bling;
using namespace bling::net;
using namespace bling::net::detail;


TimerQueue::TimerQueue(EventLoop* loop)
    : _loop(loop),
      _timerfd(createTimerfd()),
      _timerfdChannel(loop, _timerfd),
      _timers(),
      _callingExpiredTimers(false)
{
    _timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    _timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
    _timerfdChannel.disableAll();
    _timerfdChannel.remove();
    ::close(_timerfd);
    for (TimerSet::iterator it = _timers.begin(); it != _timers.end(); ++it)
    {
        LOG_TRACE << "delete timerId = " << it->second->sequence();
        delete it->second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    LOG_TRACE << "create timerId = " << timer->sequence();
    _loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
    _loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    _loop->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged)
    {
        resetTimerfd(_timerfd, timer->endTime());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    _loop->assertInLoopThread(); 
    Entry entry(timerId._timer->endTime(), timerId._timer);
    TimerSet::iterator it = _timers.find(entry);

    if (it != _timers.end())
    {
        _timers.erase(it);
        LOG_TRACE << "delete timerId = " << it->second->sequence();
        delete it->second;
    }
    else if (_callingExpiredTimers)
    {
        _cancelingTimers.insert(entry.second);
    }
}

void TimerQueue::handleRead()
{
    _loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(_timerfd, now);

    std::vector<Entry> expired = getExpired(now);

    _callingExpiredTimers = true;
    _cancelingTimers.clear();

    for (std::vector<Entry>::iterator iter = expired.begin(); iter != expired.end(); ++iter) 
    {   
        iter->second->run();
    }  
    _callingExpiredTimers = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerSet::iterator end = _timers.lower_bound(sentry);
    BLING_ASSERT(end == _timers.end() || now < end->first);
    std::copy(_timers.begin(), end, std::back_inserter(expired));
    _timers.erase(_timers.begin(), end);

    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for (std::vector<Entry>::const_iterator iter = expired.begin(); iter != expired.end(); ++iter) 
    {   
        if (iter->second->repeat() && _cancelingTimers.find(iter->second) == _cancelingTimers.end()) 
        {
            iter->second->restart(now);
            insert(iter->second);
        }
        else
        {
            LOG_TRACE << "delete timerId = " << iter->second->sequence();
            delete iter->second;
        }
    }  

    if (!_timers.empty())
    {
        nextExpire = _timers.begin()->second->endTime();
    }

    if (nextExpire.valid())
    {
        resetTimerfd(_timerfd, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer)
{
   _loop->assertInLoopThread(); 
   bool earliestChanged = false;
   Timestamp when = timer->endTime();
   TimerSet::iterator it = _timers.begin();
   if (it == _timers.end() || when < it->first)
   {
       earliestChanged = true;
   }
   std::pair<TimerSet::iterator, bool> result = _timers.insert(Entry(when, timer));
   BLING_ASSERT(result.second);

   return earliestChanged;
}




























