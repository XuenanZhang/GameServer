#include "log/AsyncLogger.h"
#include "log/LogFile.h"
#include "common/Timestamp.h"


using namespace bling;


AsyncLogger::AsyncLogger(const string& basename, off_t rollSize, int flushInterval)
    : _flushInterval(flushInterval),
      _running(false),
      _basename(basename),
      _rollSize(rollSize),
      _thread(std::bind(&AsyncLogger::threadFunc, this), "Logger"),
      _latch(1),
      _mutex(),
      _cond(_mutex),
      _currBuffer(new Buffer),
      _nextBuffer(new Buffer),
      _buffers()
{
    _currBuffer->bzero();
    _nextBuffer->bzero();
    _buffers.reserve(16);
}
AsyncLogger::~AsyncLogger()
{
    if (_running)
    {
        stop();
    }
}

void AsyncLogger::append(const char* logline, int len)
{
    MutexLockAuto lock(_mutex);
    if (_currBuffer->avail() > len)
    {
        _currBuffer->append(logline, len);
    }
    else
    {
        _buffers.push_back(std::move(_currBuffer));

        if (_nextBuffer)
        {
            _currBuffer = std::move(_nextBuffer);
        }
        else
        {
            _currBuffer.reset(new Buffer);
        }
        _currBuffer->append(logline, len);
        _cond.notify();
    }
}

void AsyncLogger::start()
{
    _running = true;
    _thread.start();
    _latch.wait();
}

void AsyncLogger::stop()
{
    _running = false;
    _cond.notify();
    _thread.join();
}

void AsyncLogger::threadFunc()
{
    BLING_ASSERT(_running);
    _latch.countDown();
    LogFile output(_basename, _rollSize);
    std::unique_ptr<Buffer> newBuffer1(new Buffer);
    std::unique_ptr<Buffer> newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    while (_running)
    {
        BLING_ASSERT(newBuffer1 && newBuffer1->length() == 0);
        BLING_ASSERT(newBuffer2 && newBuffer1->length() == 0);
        BLING_ASSERT(buffersToWrite.empty());

        {
            MutexLockAuto lock(_mutex);
            if (_buffers.empty())
            {
                _cond.waitFoSeconds(_flushInterval);
            }

            _buffers.push_back(std::move(_currBuffer));
            _currBuffer = std::move(newBuffer1);
            buffersToWrite.swap(_buffers);
            if (!_nextBuffer)
            {
                _nextBuffer = std::move(newBuffer2);
            }
        }

        BLING_ASSERT(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd buffers",
                     Timestamp::now().toFormatString().c_str(),
                     buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i) 
        {   
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        } 

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            BLING_ASSERT(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite[buffersToWrite.size() - 1]);
            newBuffer1->reset();
            buffersToWrite.pop_back();
        }

        if (!newBuffer2)
        {
            BLING_ASSERT(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite[buffersToWrite.size() - 1]);
            newBuffer2->reset();
            buffersToWrite.pop_back();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
