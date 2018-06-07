/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-16 09:36
 * Last modified : 2018-04-16 09:36
 * Filename      : CurrentThread.h

 * Description   : 当前线程信息（每个线程独一份）
 * *******************************************************/
#ifndef _BLING_CURRENTTHREAD_H_
#define _BLING_CURRENTTHREAD_H_

#include <stdint.h>

namespace bling
{

namespace CurrentThread
{

extern __thread int t_cachedTid; 
extern __thread char t_tidString[32]; 
extern __thread int t_tidStringLength; 
extern __thread const char* t_threadName; 
extern __thread uint32_t t_randseed; 

void cacheTid();
void cacheRandseed();

inline int tid()
{
    if ( t_cachedTid == 0)
    {
        cacheTid();
    }

    return t_cachedTid;
}

inline const char* tidString() { return t_tidString; }

inline const int tidStringLength() { return t_tidStringLength; }

inline const char* name() { return t_threadName; }

bool isMainThread();

void sleepUsec(int64_t usec);


}; //CurrentThread

}; //bling


#endif //_BLING_CURRENTTHREAD_H_
