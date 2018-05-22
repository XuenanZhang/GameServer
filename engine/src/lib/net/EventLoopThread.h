/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-18 17:52
 * Last modified : 2018-05-18 17:52
 * Filename      : EventLoopThread.h

 * Description   : 事件线程封装
 * *******************************************************/
#ifndef _BLING_EVENTLOOPTHREAD_H_
#define _BLING_EVENTLOOPTHREAD_H_

#include "common/noncopyable.h"
#include "common/Condition.h"
#include "common/Mutex.h"
#include "thread/Thread.h"

namespace bling
{

namespace net 
{

class EventLoop;

class EventLoopThread : bling::noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), 
                    const string& name = string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* _loop;
    bool _exiting;
    Thread _thread;
    MutexLock _mutex;
    Condition _cond;
    ThreadInitCallback _callback;

}; // class EventLoopThread

}; // ns net

}; //ns bling


#endif // _BLING_EVENTLOOPTHREAD_H_
