#include "EventLoop.h"
#include "log/Logger.h"
#include "common/Mutex.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/SocketAPI.h"
#include "net/TimerQueue.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <algorithm>

using namespace bling;
using namespace bling::net;

namespace  
{

__thread EventLoop* t_loopInThisThread = NULL;

const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }

    return evtfd;
}

// #include <signal.h>
// class IgnoreSigPipe 
// {
// public:
    // IgnoreSigPipe()
    // {
        // ::signal(SIGPIPE, SIG_IGN);
    // }
// }; // class IgnoreSigPipe

}; // ns 

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

EventLoop::EventLoop()
        : _looping(false),
          _quit(false),
          _eventHandling(false),
          _callingPendingFunctors(false),
          _iteration(0),
          _threadId(CurrentThread::tid()),
          _poller(Poller::newDefaultPoller(this)),
          _timerQueue(new TimerQueue(this)),
          _wakeupFd(createEventfd()),
          _wakeupChannel(new Channel(this, _wakeupFd)),
          _currentActiveChannel(NULL)
{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << _threadId;
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << _threadId;
    }
    else
    {
        t_loopInThisThread = this; 
    }

    _wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
    LOG_DEBUG << "EventLoop " << this << " of thread " << _threadId << " destructs in thread" << CurrentThread::tid();
    _wakeupChannel->disableAll();
    _wakeupChannel->remove();
    ::close(_wakeupFd);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    BLING_ASSERT(!_looping);
    assertInLoopThread();
    _looping = true;
    _quit = false;
    LOG_TRACE << "EventLoop " << this << " start loop";

    while (!_quit)
    {
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(kPollTimeMs, &_activeChannels);
        ++_iteration;

        if (Logger::LogLevel() <= Logger::TRACE)
        {
            printActiveChannels();
        }
        _eventHandling = true;
        for (ChannelVector::iterator it = _activeChannels.begin(); it != _activeChannels.end(); ++it)
        {
            _currentActiveChannel = *it; 
            _currentActiveChannel->handleEvent(_pollReturnTime);
        }

        _currentActiveChannel = NULL;
        _eventHandling = false;

        //处理挂起的函数
        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    _looping = false;
}

void EventLoop::quit()
{
    _quit = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockAuto lock(_mutex);
        _pendingFunctors.push_back(cb);
    }

    if (!isInLoopThread() || _callingPendingFunctors)
    {
        wakeup();
    }
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return _timerQueue->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return _timerQueue->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    return _timerQueue->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
    BLING_ASSERT(channel->ownerLoop() == this);
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    BLING_ASSERT(channel->ownerLoop() == this);
    assertInLoopThread();
    if (_eventHandling)
    {
        BLING_ASSERT(_currentActiveChannel == channel || std::find(_activeChannels.begin(), _activeChannels.end(), channel) == _activeChannels.end()); 
    }

    _poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    BLING_ASSERT(channel->ownerLoop() == this) ;
    assertInLoopThread();
    return _poller->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
   LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this << " was created in _thread = " << _threadId << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(_wakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(_wakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead reads " << n << "bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    
    {
        MutexLockAuto lock(_mutex);
        functors.swap(_pendingFunctors);
    }

    for (size_t i = 0; i < functors.size(); ++i) 
    {   
        functors[i]();
    }   

    _callingPendingFunctors = false;
}

void EventLoop::printActiveChannels() const
{
    for (ChannelVector::const_iterator it = _activeChannels.begin(); it != _activeChannels.end(); ++it) 
    {   
        const Channel* ch = *it;
        LOG_TRACE << "{" << ch->reventsToString() << "}";
    }  
}



























