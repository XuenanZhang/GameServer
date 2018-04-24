#ifndef _BLING_LOGFILE_H_
#define _BLING_LOGFILE_H_

#include "common/noncopyable.h"
#include "common/Type.h"

#include <memory>

namespace bling
{

namespace FileUtil
{
class AppendFile;
}

class LogFile : bling::noncopyable
{
public:
    LogFile(const string& string, off_t rollSize);
    ~LogFile();

    void append(const char* logline, int len);

    void flush();

    bool rollFile();

private:
    string getLogFileName();

private:
    const string _basename;
    const off_t _rollSize; 

    std::unique_ptr<FileUtil::AppendFile> _file;
}; // class LogFile

}; //ns bling


#endif // _BLING_LOGFILE_H_
