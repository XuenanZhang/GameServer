/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-11 16:13
 * Last modified : 2018-04-11 16:13
 * Filename      : Mutex.h

 * Description   : 互斥锁
 * *******************************************************/
#ifndef _BLING_MUTEX_H_
#define _BLING_MUTEX_H_

#include "common/noncopyable.h"

#include <pthread.h>

namespace bling
{

class MutexLock : public bling::noncopyable
{
public:
    MutexLock() 
    {
        pthread_mutex_init(&_mutex, NULL);
    };
    ~MutexLock()
    {
        pthread_mutex_destroy(&_mutex); 
    };

public:
    void lock()
    {
        pthread_mutex_lock(&_mutex);
    };
    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    };

    pthread_mutex_t* getThreadMutex() { return &_mutex; };

private:
    pthread_mutex_t _mutex;
};

class MutexLockAuto
{
public:
    explicit MutexLockAuto(MutexLock& mutexLock) : _mutexLock(mutexLock)
    {
        _mutexLock.lock();
    }
    ~MutexLockAuto()
    {
        _mutexLock.unlock();
    }

private:
    MutexLock& _mutexLock;
};

}; // bling

#endif //_BLING_MUTEX_H_
