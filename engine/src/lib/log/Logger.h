/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-20 16:11
 * Last modified : 2018-04-20 16:11
 * Filename      : Logger.h

 * Description   : 单挑日志记录
 * *******************************************************/
#ifndef _BLING_LOGGER_H_
#define _BLING_LOGGER_H_

#include "log/LogStream.h"
#include "common/Timestamp.h"

#include <stdio.h>
namespace bling
{

class Logger 
{
public:
    enum LogLevel
    {
        TRACE, 
        DEBUG, 
        INFO,
        WARN, 
        ERROR, 
        FATAL, 
        MAX_LOG_LEVEL,
    };

    class SourceFile 
    {
    public:
        template<int N>
        SourceFile(const char (&arr)[N]) 
            : _data(arr), _size(N-1)
        {
            const char* slash = strrchr(_data, '/');
            if (slash)
            {
                _data = slash + 1;
                _size -= static_cast<int>(_data - arr);
            }
        }

        explicit SourceFile(const char* filename)
            : _data(filename)
        {
            const char* slash = strrchr(filename, '/');
            if (slash)
            {
                _data = slash + 1;
            }

            _size = static_cast<int>(strlen(_data));
        }

        const char* _data;
        int _size;

    }; // class SourceFile

public:
    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return _impl._stream;}

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc fun);
    static void setFlush(FlushFunc fun);

private:
    class Impl 
    {
    public:
        // typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);

        void formatTime();
        void finish();

        Timestamp _time;
        LogStream _stream;
        LogLevel _level;
        int _line;
        SourceFile _basename;
    }; // class Impl

    Impl _impl;

}; // class logger

extern Logger::LogLevel g_logLevel;


#define LOG_TRACE if (bling::Logger::logLevel() <= bling::Logger::TRACE) \
    bling::Logger(__FILE__, __LINE__, bling::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (bling::Logger::logLevel() <= bling::Logger::DEBUG) \
    bling::Logger(__FILE__, __LINE__, bling::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (bling::Logger::logLevel() <= bling::Logger::INFO) \
    bling::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN if (bling::Logger::logLevel() <= bling::Logger::WARN) \
    bling::Logger(__FILE__, __LINE__, bling::Logger::WARN).stream()
#define LOG_ERROR bling::Logger(__FILE__, __LINE__, bling::Logger::ERROR).stream()
#define LOG_FATAL bling::Logger(__FILE__, __LINE__, bling::Logger::FATAL).stream()
#define LOG_SYSERR bling::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL bling::Logger(__FILE__, __LINE__, true).stream()





const char* strerror_tl(int savedErrno);

#define CHECK_NOTNULL(val) \
    ::bling::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL ", (val))

template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char* names, T* ptr)
{
    if (ptr == NULL)
    {
        Logger(file, line, Logger::FATAL).stream() << names;
    }

    return ptr;
}




















}; //ns bling


#endif //_BLING_LOGGER_H_
