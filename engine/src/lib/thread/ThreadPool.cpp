#include "ThreadPool.h"

#include <algorithm>

using namespace bling;

ThreadPool::ThreadPool(const string& nameArg)
    : _mutex(), _notEmpty(_mutex), _notFull(_mutex), _name(nameArg), _running(false), _maxQueueSize(0)
{
}

ThreadPool::~ThreadPool()
{
    if (_running)
    {
        stop();
    }

    std::for_each(_threads.begin(), _threads.end(), [] (bling::Thread* t) { delete t;});
}

void ThreadPool::start(int32_t numThreads)
{
    BLING_ASSERT(_threads.empty());
    _running = true;
    _threads.reserve(numThreads);
    for (int32_t i = 0; i < numThreads; ++i) 
    {   
        char id[32];
        snprintf(id, sizeof(id), "%d", i+1);
        _threads.push_back(new bling::Thread(std::bind(&ThreadPool::runInThread, this), _name + id));
        _threads[i]->start();
    }   

    if (numThreads == 0 && _threadInitCallback)
    {
        _threadInitCallback();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockAuto lock(_mutex);
        _running = false;
        _notEmpty.notifyAll();
    }

    std::for_each( _threads.begin(), _threads.end(), std::bind(&bling::Thread::join, std::placeholders::_1));
}

void ThreadPool::run(const Task& task)
{
    if (_threads.empty())
    {
        task();
    }
    else 
    {
        MutexLockAuto lock(_mutex);

        while (isFull())
        {
            _notFull.wait();
        }

        BLING_ASSERT(!isFull());

        _queue.push_back(task);
        _notEmpty.notify();
    }
}

void ThreadPool::runInThread()
{
    if (_threadInitCallback)
    {
        _threadInitCallback();
    }

    while (_running)
    {
        Task task(getTask());
        if (task)
        {
            task();
        }
    }
}

ThreadPool::Task ThreadPool::getTask()
{
    MutexLockAuto lock(_mutex);

    while(_queue.empty() && _running)
    {
        _notEmpty.wait();
    }

    Task task;

    if (!_queue.empty())
    {
        task = _queue.front();
        _queue.pop_front();

        if (_maxQueueSize > 0)
        {
            _notFull.notify();
        }
    }

    return task;
}

bool ThreadPool::isFull() const
{
    return _maxQueueSize > 0 && _queue.size() >= _maxQueueSize;
}





























