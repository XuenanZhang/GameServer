/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-13 17:37
 * Last modified : 2018-04-13 07:37
 * Filename      : CountDownLatch.h

 * Description   : 线程同步计数器
 * *******************************************************/
#ifndef _BLING_COUNTDOWNLATCH_H_
#define _BLING_COUNTDOWNLATCH_H_

#include "common/noncopyable.h"
#include "common/Mutex.h"
#include "common/Condition.h"

namespace bling
{

class CountDownLatch : bling::noncopyable
{
public:
    explicit CountDownLatch( int count );

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable MutexLock _mutex;
    Condition _condition;
    int _count;
};

}; //bling


#endif //_BLING_COUNTDOWNLATCH_H_
