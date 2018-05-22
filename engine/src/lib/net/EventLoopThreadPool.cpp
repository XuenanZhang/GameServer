#include "net/EventLoopThreadPool.h"

#include "net/EventLoop.h"
#include "net/EventLoopThread.h"


using namespace bling;
using namespace bling::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
    : _baseLoop(baseLoop),
      _name(nameArg),
      _started(false),
      _numThreads(0),
      _next(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    BLING_ASSERT(!_started);
    _baseLoop->assertInLoopThread();

    _started = true;

    for (int32_t i = 0; i < _numThreads; ++i) 
    {   
        char buf[_name.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", _name.c_str(), i);
        std::unique_ptr<EventLoopThread> uptr(new EventLoopThread(cb, buf));
        _loops.push_back(uptr->startLoop());
        _threads.push_back(std::move(uptr));
    }   

    if (_numThreads == 0 && cb)
    {
        cb(_baseLoop);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    _baseLoop->assertInLoopThread();
    BLING_ASSERT(_started);
    EventLoop* loop = _baseLoop;

    if (!_loops.empty())
    {
        loop = _loops[_next];
        ++_next;
        if (static_cast<size_t>(_next) >= _loops.size())
        {
            _next = 0;
        }
    }

    return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hasCode)
{
    _baseLoop->assertInLoopThread();
    EventLoop* loop = _baseLoop;

    if (!_loops.empty())
    {
        loop = _loops[hasCode % _loops.size()];
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    _baseLoop->assertInLoopThread();
    BLING_ASSERT(_started);

    if (_loops.empty())
    {
        return std::vector<EventLoop*>(1, _baseLoop);
    }
    else
    {
        return _loops; 
    }
}
