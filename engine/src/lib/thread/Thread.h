/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-16 12:01
 * Last modified : 2018-04-16 12:01
 * Filename      : Thread.h

 * Description   : 线程类
 * *******************************************************/
#ifndef _BLING_THREAD_H_
#define _BLING_THREAD_H_

#include "common/noncopyable.h"
#include "common/Type.h"
#include "thread/CurrentThread.h"
#include "common/CountDownLatch.h"

#include <pthread.h>
#include <atomic>
#include <utility>
#include <functional>

namespace bling
{

class Thread : bling::noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread( const ThreadFunc , const string& name = string());
    ~Thread();

public:
    void start();

    int join();

    bool started() const { return _started; }

    pid_t tid() const { return _tid; }

    const string& name() const { return _name; }

    static int numCreated() { return _s_numCreated.load(); }

private:
    void setDefaultName();

private:

    bool           _started;
    bool           _joined;
    pthread_t      _pthreadId;
    pid_t          _tid;
    ThreadFunc     _func;
    string         _name;
    CountDownLatch _latch;

    static std::atomic<int32_t> _s_numCreated;
};

}; //bling


#endif // _BLING_THREAD_H_
