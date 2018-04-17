/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-11 16:28
 * Last modified : 2018-04-11 16:28
 * Filename      : Condition.h

 * Description   : 条件变量
 * *******************************************************/
#ifndef _BLING_CONDITION_H_
#define _BLING_CONDITION_H_

#include "common/noncopyable.h"
#include "common/Mutex.h"

#include <pthread.h>

namespace bling
{

class Condition : public bling::noncopyable
{
public:
    explicit Condition(MutexLock& mutex) : _mutex(mutex)
    {
        pthread_cond_init(&_pCond, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&_pCond);
    }

public:
    void wait()
    {
        pthread_cond_wait(&_pCond, _mutex.getThreadMutex());
    }
    
    bool waitFoSeconds(double seconds);

    void notify()
    {
        pthread_cond_signal(&_pCond);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&_pCond);
    }

private:
    MutexLock& _mutex;
    pthread_cond_t _pCond;
    
};

}; //bling


#endif //_BLING_CONDITION_H_
