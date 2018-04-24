/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-24 17:17
 * Last modified : 2018-04-24 17:17
 * Filename      : AsyncLogger.h

 * Description   : 异步日志
 * *******************************************************/
#ifndef _BLING_ASYNCLOGGER_H_
#define _BLING_ASYNCLOGGER_H_

#include "common/noncopyable.h"
#include "common/Type.h"
#include "LogStream.h"
#include "thread/Thread.h"
#include "common/CountDownLatch.h"
#include "common/Mutex.h"
#include "common/Condition.h"

#include <memory>
#include <vector>

namespace bling
{

class AsyncLogger : bling::noncopyable
{
public:
    AsyncLogger(const string& basename, off_t rollSize, int flushInterval = 3);
    ~AsyncLogger();

    void append(const char* logline, int len);

    void start();

    void stop();

private:
    void threadFunc();

    typedef bling::detail::FixedBuff<bling::detail::kBigBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;

    const int _flushInterval;
    bool _running;
    string _basename;
    off_t _rollSize;
    bling::Thread _thread;
    bling::CountDownLatch _latch;
    bling::MutexLock _mutex;
    bling::Condition _cond;
    std::unique_ptr<Buffer> _currBuffer;
    std::unique_ptr<Buffer> _nextBuffer;
    BufferVector _buffers;

}; // class AsyncLogger

}; //ns bling


#endif // _BLING_ASYNCLOGGER_H_
