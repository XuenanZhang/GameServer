/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-21 14:26
 * Last modified : 2018-05-21 14:26
 * Filename      : EventLoopThreadPool.h

 * Description   : 事件线程池
 * *******************************************************/
#ifndef _BLING_EVENTLOOPTHREADPOOL_H_
#define _BLING_EVENTLOOPTHREADPOOL_H_

#include "common/Type.h"
#include "common/noncopyable.h"

#include <vector>
#include <functional>
#include <memory>

namespace bling
{

namespace net 
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : bling::noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { _numThreads = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();

    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return _started; }

    const string& name() const { return _name; }

private:
    EventLoop* _baseLoop;
    string _name;
    bool _started;
    int _numThreads;
    int _next;
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;

}; // class EventLoopThreadPool

}; // ns net

}; //ns bling


#endif // _BLING_EVENTLOOPTHREADPOOL_H_
