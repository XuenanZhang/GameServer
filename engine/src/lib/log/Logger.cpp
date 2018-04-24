#include "log/Logger.h"
#include "thread/CurrentThread.h"
#include "common/Date.h"

#include <errno.h>
// #include <stdio.h>


namespace bling 
{

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logger::LogLevel initLogLevel()
{
    if (::getenv("BLING_LOG_TRACE"))
        return Logger::TRACE;
    else if (::getenv("BLING_LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::TRACE;
}
Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::MAX_LOG_LEVEL] = 
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

class T 
{
public:
    T(const char* str, unsigned len)
        :_str(str), _len(len)
    {
        BLING_ASSERT(strlen(str) == len);
    }

    const char* _str;
    const unsigned _len;
}; // class T

inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v._str, v._len);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v._data, v._size);
    return s;
}

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;


}; // ns bling

using namespace bling;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
    : _time(Timestamp::now()), _stream(), _level(level), _line(line), _basename(file)
{
    formatTime();
    CurrentThread::tid();
    _stream << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    _stream << LogLevelName[level];
    // _stream << T(LogLevelName[level], 6);
    if (savedErrno != 0)
    {
        _stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}

void Logger::Impl::formatTime()
{
    int64_t microSecondsSinceEpoch = _time.getMircoSeconds();
    time_t seconds = _time.getSeconds();
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondPerSecond);

    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        Date date;
        snprintf(t_time, sizeof t_time, "%4d-%02d-%02d %02d:%02d:%02d",
            date.year(), date.month(), date.day(), date.hour(), date.minutes(), date.seconds());
    }

    Fmt us(".%06d ", microseconds);
    BLING_ASSERT(us.length() == 8);
    _stream << T(t_time, 19) << T(us.data(), us.length());
}

void Logger::Impl::finish()
{
    _stream << " - " << _basename << ":" << _line << '\n';
}

Logger::Logger(SourceFile file, int line)
    :_impl(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
    :_impl(level, 0, file, line)
{
    _impl._stream<< func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
    :_impl(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
    :_impl(toAbort ? FATAL : ERROR, errno, file, line)
{
}

Logger::~Logger()
{
    _impl.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());

    if (_impl._level == FATAL)
    {
        g_flush();
        // abort();
    }
}

Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc func)
{
    g_output = func;
}

void Logger::setFlush(FlushFunc func)
{
    g_flush = func;
}









