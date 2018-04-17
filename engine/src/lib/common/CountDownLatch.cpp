#include "common/CountDownLatch.h"

using namespace bling;

CountDownLatch::CountDownLatch(int count) 
        : _mutex(), 
          _condition(_mutex), 
          _count(count)
{
}

void CountDownLatch::wait()
{
    MutexLockAuto lock(_mutex);
    while(_count > 0)
    {
        _condition.wait();
    }
}

void CountDownLatch::countDown()
{
    MutexLockAuto lock(_mutex);
    --_count;
    if (_count == 0)
    {
        _condition.notifyAll();
    }
}

int CountDownLatch::getCount() const
{
    MutexLockAuto lock(_mutex);
    return _count;
}
