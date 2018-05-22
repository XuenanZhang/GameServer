#include "thread/Thread.h"
#include "common/Timestamp.h"
#include "log/Logger.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
// #include <sys/types.h>
// #include <linux/unistd.h>

namespace bling 
{

namespace CurrentThread
{
    __thread int32_t t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "unkonwn";
}; // ns CurrentThread

namespace detail 
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

struct ThreadData
{
    Thread::ThreadFunc _func;
    string _name;
    pid_t* _tid;
    CountDownLatch* _latch;

    ThreadData(const Thread::ThreadFunc& func,
                const string& name,
                pid_t* tid,
                CountDownLatch* latch)
        : _func(func), _name(name), _tid(tid), _latch(latch) 
    {
    }

    void runInThread()
    {
        *_tid = CurrentThread::tid();
        _tid = NULL;
        _latch->countDown();
        _latch = NULL;

        CurrentThread::t_threadName = _name.empty() ? "blingThread" : _name.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);

        _func();
        // try
        // {
            // _func();
        // }
        // catch (...)
        // {
            // throw; 
        // }
    }
}; // struct ThreadData

void* startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

}; // ns detail

}; // ns bling


using namespace bling;

// 进程pid: getpid()                 
// 线程tid: pthread_self()     //进程内唯一，但是在不同进程则不唯一。
// 线程pid: syscall(SYS_gettid)     //系统内是唯一的
void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0,0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

std::atomic<int32_t> Thread::_s_numCreated(0);
Thread::Thread(const ThreadFunc& func, const string& nameArg)
        : _started(false), _joined(false), _pthreadId(0), _tid(0), _func(func), _name(nameArg), _latch(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if ( _started && !_joined)
    {
        pthread_detach(_pthreadId);
    }

    // LOG_INFO << "thread distory";
}

void Thread::start()
{
    BLING_ASSERT(!_started);
    _started = true;
    detail::ThreadData * data = new detail::ThreadData(_func, _name, &_tid, &_latch);

    if (pthread_create(&_pthreadId, NULL, &detail::startThread, data))
    {
        _started = false;
        delete data;
        // LOG_SYSFATAL << "Failed in pthread_create";
    }
    else
    {
        _latch.wait();
        BLING_ASSERT(_tid > 0);
    }
}

int Thread::join()
{
    BLING_ASSERT(_started);
    BLING_ASSERT(!_joined);
    _joined = true;
    return pthread_join(_pthreadId, NULL);
}

void Thread::setDefaultName()
{
    int num = _s_numCreated++;
    if (_name.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        _name = buf;
    }
}












