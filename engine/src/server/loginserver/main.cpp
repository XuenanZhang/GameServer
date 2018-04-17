#include <stdio.h>
#include <unistd.h>

#include "thread/Thread.h"
#include "thread/ThreadPool.h"
#include "common/Mutex.h"
#include "common/CountDownLatch.h"
using namespace bling;

MutexLock mutex;
int num = 0;
void threadFun()
{
    MutexLockAuto lock(mutex);
    num++;
    printf("threadId=%d threadName=%s ==> num = %d\n", CurrentThread::tid(), CurrentThread::name(), num);
}

void testThread()
{
    CountDownLatch latch(1);
    // Thread thread1(threadFun);
    // thread1.start();
    ThreadPool tp("threadPool");
    tp.setMaxQueueSize(5);
    tp.start(5);
    for (int32_t i = 0; i < 100; ++i) 
    {   
       tp.run(threadFun);
    }   

    // usleep(1000*1000*5);
    tp.stop();
    printf("end.................\n");
}

int main()
{
    testThread();
    return 0;
}
