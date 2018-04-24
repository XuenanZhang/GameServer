/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-18 11:54
 * Last modified : 2018-04-18 11:54
 * Filename      : BlockingQueue.h

 * Description   : 阻塞队列
 * *******************************************************/
#ifndef _BLING_BLOCKINGQUEUE_H_
#define _BLING_BLOCKINGQUEUE_H_

#include "common/noncopyable.h"
#include "common/Mutex.h"
#include "common/Condition.h"

#include <deque>

namespace bling
{

template<typename T>
class BlockingQueue : bling::noncopyable
{
public:
    BlockingQueue() : _mutex(), _notEmpty(_mutex), _queue()
    {
    }

    void put(const T& x)
    {
        MutexLockAuto lock(_mutex);
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

        return front;
    }

    size_t size() const
    {
        MutexLockAuto lock(_mutex);
        return _queue.size();
    }


private:
    MutexLock _mutex;
    Condition _notEmpty;
    std::deque<T> _queue;
}; // class BlockingQueue

}; //bling


#endif // _BLING_BLOCKINGQUEUE_H_
