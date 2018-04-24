#ifndef _BLING_BOUNDEDBLOCKINGQUEU_H_
#define _BLING_BOUNDEDBLOCKINGQUEU_H_

#include "common/noncopyable.h"
#include "common/Mutex.h"
#include "common/Condition.h"

#include <deque>

namespace bling
{

template<typename T>
class BoundBlockingQueue : bling::noncopyable
{
public:
    BoundBlockingQueue(int maxSize) : _mutex(), _notEmpty(_mutex), _queue(), _maxSize(maxSize)
    {
    }

    void put(const T& x)
    {
        MutexLockAuto lock(_mutex);

        while (_queue.size() >= _maxSize)
        {
            _notFull.wait(); 
        }

        BLING_ASSERT(_queue.size() < _maxSize);
        _queue.push_back(x);
        _notEmpty.notify();
    }

    T take()
    {
        MutexLockAuto lock(_mutex);

        while(_queue.empty())
        {
            _notEmpty.wait();
        }

        BLING_ASSERT(!_queue.empty());

        T front(_queue.front());
        _queue.pop_front();

        _notFull.notify();

        return front;
    }

    bool empty() const
    {
        MutexLockAuto lock(_mutex);
        return _queue.empty();
    }

    bool full() const
    {
        MutexLockAuto lock(_mutex);
        return _queue.size() >= _maxSize;
    }

    size_t size() const
    {
        MutexLockAuto lock(_mutex);
        return _queue.size();
    }


private:
    MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;
    std::deque<T> _queue;
    int _maxSize;


}; // class BoundBlockingQueue

}; //bling


#endif // _BLING_BOUNDEDBLOCKINGQUEU_H_
