#ifndef _BLING_LOGGER_H_
#define _BLING_LOGGER_H_

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

public:
    Logger(){};
    ~Logger(){};

public:
    Logger& operator<<(const char* s){ printf("----> %s \n", s);return *this; };
};


#define LOG_TRACE Logger()
#define LOG_DEBUG Logger()
#define LOG_INFO Logger()
#define LOG_WARN Logger()
#define LOG_ERROR Logger()
#define LOG_FATAL Logger()
#define LOG_SYSERR Logger()
#define LOG_SYSFATAL Logger()


}; //bling


#endif //_BLING_LOGGER_H_
