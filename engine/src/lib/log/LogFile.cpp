#include "log/LogFile.h"
#include "common/Date.h"
#include "common/FileUtil.h"
#include "common/ProcessInfo.h"


using namespace bling;

LogFile::LogFile(const string& basename, off_t rollSize)
    : _basename(basename), _rollSize(rollSize)
{
    rollFile();    
}

LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
    _file->append(logline, len);

    if (_file->writtenBytes() > _rollSize)
    {
        rollFile();
    }
    else
    {
        
    }
}

void LogFile::flush()
{
    _file->flush();
}

bool LogFile::rollFile()
{
    string filename = getLogFileName();

    _file.reset(new FileUtil::AppendFile(filename));
    return true;
}

string LogFile::getLogFileName()
{
    string filename;
    filename.reserve(_basename.size() + 64);
    filename = _basename;

    char timebuf[32];
    Date date;
    snprintf(timebuf, sizeof timebuf, "_%04d%02d%02d-%02d%02d%02d.", date.year(), date.month(), date.day(),date.hour(), date.minutes(), date.seconds());
    filename += timebuf;

    filename += ProcessInfo::hostname();
    //
    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;
    
    filename += ".log";

    return filename;
}





