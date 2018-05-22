#include "net/EventLoopThread.h"

#include "net/EventLoop.h"

using namespace bling;
using namespace bling::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const string& name)
    : _loop(NULL),
      _exiting(false),
      _thread(std::bind(&EventLoopThread::threadFunc, this), name),
      _mutex(),
      _cond(_mutex),
      _callback(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    _exiting = true;
    if (_loop != NULL)
    {
        _loop->quit();
        _thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    BLING_ASSERT(!_thread.started());

    _thread.start();

    {
        MutexLockAuto lock(_mutex);
        while (_loop == NULL)
        {
            _cond.wait();
        }
    }

    return _loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if (_callback)
    {
        _callback(&loop);
    }

    
    {
        MutexLockAuto lock(_mutex);
        _loop = &loop;
        _cond.notify();
    }

    loop.loop();
    _loop = NULL;
}










