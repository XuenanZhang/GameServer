/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-16 17:56
 * Last modified : 2018-04-17 15:23
 * Filename      : ThreadPool.h

 * Description   : 
 * *******************************************************/
#ifndef _BLING_THREADPOOL_H_
#define _BLING_THREADPOOL_H_

#include "common/noncopyable.h"
#include "common/Mutex.h"
#include "common/Condition.h"
#include "thread/Thread.h"
#include "common/Type.h"

#include <functional>
#include <vector>
#include <queue>
#include <memory>

namespace bling
{

class ThreadPool : public bling::noncopyable 
{
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const string& nameArg);
    ~ThreadPool();

public:
    //设置以后需要创建线程池的线程调用 run
    void setMaxQueueSize(int maxSize) { _maxQueueSize = maxSize; }

    void setThreadInitCallback(const Task& cb) { _threadInitCallback = cb; }

    void start(int32_t numThreads);

    void stop();

    void run(const Task& f);

    const string& name() const { return _name; }

private:
    void runInThread();
    Task getTask();
    bool isFull() const; 

private:
    mutable MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;
    string _name;
    Task _threadInitCallback;
    std::vector<std::unique_ptr<bling::Thread>> _threads;
    std::vector<int> _threadss;
    std::deque<Task> _queue;
    bool _running;
    size_t _maxQueueSize;

}; // class ThreadPool

}; //bling


#endif // _BLING_THREADPOOL_H_
