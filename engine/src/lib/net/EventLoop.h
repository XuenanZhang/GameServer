/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-09 17:42
 * Last modified : 2018-05-09 17:42
 * Filename      : EventLoop.h

 * Description   : 事件循环（每个线程只能有一个）
 * *******************************************************/
#ifndef _BLING_EVENTLOOP_H_
#define _BLING_EVENTLOOP_H_

#include "common/noncopyable.h"
#include "common/Timestamp.h"
#include "common/Mutex.h"
#include "thread/CurrentThread.h"
#include "net/TimerId.h"
#include "net/Callbacks.h"

#include <functional>
#include <memory>

namespace bling
{

namespace net 
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop : bling::noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    /** 必须在创建此对象的线程运行 **/
    void loop();

    void quit();// unsafe

    /** poll阻塞时间 **/
    Timestamp pollReturnTime() const { return _pollReturnTime; }

    /** 总共wait次数 **/
    int64_t iteration() const { return _iteration; }

    void runInLoop(const Functor& cb);// safe

    /** 在此线程运行回调函数 **/
    void queueInLoop(const Functor& cb);

    size_t queueSize() const;

    /** 时间回调 **/
    TimerId runAt(const Timestamp& time, const TimerCallback& cb);

    TimerId runAfter(double delay, const TimerCallback& cb);

    TimerId runEvery(double interval, const TimerCallback& cb);

    void cancel(TimerId timerId);

    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }
    
    bool isInLoopThread() const { return _threadId == CurrentThread::tid(); }
    bool eventHandling() const { return _eventHandling; }

    void setContext(void* context) { _context = context; }
    void* getContext() { return _context; }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    void handleRead();

    void doPendingFunctors();

    void printActiveChannels() const;

    typedef std::vector<Channel*> ChannelVector;

    bool _looping;
    bool _quit;
    bool _eventHandling;
    bool _callingPendingFunctors;
    int64_t _iteration;
    const pid_t _threadId;
    Timestamp _pollReturnTime;
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerQueue> _timerQueue;
    int _wakeupFd;
    std::unique_ptr<Channel> _wakeupChannel;
    void * _context;

    ChannelVector _activeChannels;
    Channel* _currentActiveChannel;

    mutable MutexLock _mutex;
    std::vector<Functor> _pendingFunctors;
}; // class EventLoop

}; // ns net

}; //ns bling


#endif // _BLING_EVENTLOOP_H_
