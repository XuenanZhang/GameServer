#include <errno.h>
#include "Condition.h"
#include "Type.h"

bool bling::Condition::waitFoSeconds(double seconds)
{
    struct timespec abstime;    
    clock_gettime(CLOCK_REALTIME, &abstime);

    const int64_t nanoPer = 1000000000;
    int64_t nanoseconds = static_cast<int64_t>(nanoPer * seconds);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / nanoPer);
    abstime.tv_nsec = static_cast<time_t>((abstime.tv_nsec + nanoseconds) % nanoPer);


    return ETIMEDOUT == pthread_cond_timedwait(&_pCond, _mutex.getThreadMutex(), &abstime);
}

