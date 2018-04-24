#include "common/FileUtil.h"
#include "log/Logger.h"

// #include <errno.h>
// #include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>



using namespace bling;

int FileUtil::readFile(string filename, int maxSize, string* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime)
{
    FileUtil::ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

FileUtil::ReadSmallFile::ReadSmallFile(string filename)
    : _fd(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), _err(0)
{
   _buff[0] = '\0' ;
   if (_fd < 0)
   {
       _err = errno;
       LOG_SYSERR << "ReadSmallFile ::open";
   }
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
    if (_fd >= 0)
    {
        ::close(_fd);
    }
}

int FileUtil::ReadSmallFile::readToString(int maxSize, string* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime)
{
    BLING_ASSERT(content != NULL);

    int err = _err;
    if (_fd > 0)
    {
        content->clear();

        if (fileSize)
        {
            struct stat statbuf;
            if (::fstat(_fd, &statbuf) == 0)
            {
                if (S_ISREG(statbuf.st_mode)) //一般文件
                {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
                }
                else if (S_ISDIR(statbuf.st_mode)) //文件夹
                {
                    err = EISDIR;
                }

                if (modifyTime)
                {
                    *modifyTime = statbuf.st_mtime;
                }

                if (createTime)
                {
                    *createTime = statbuf.st_ctime;
                }
            }
            else
            {
                err = errno; 
            }
        }

        while (content->size() < static_cast<size_t>(maxSize))
        {
            size_t toRead = std::min(static_cast<size_t>(maxSize) - content->size(), sizeof _buff);
            ssize_t n = ::read(_fd, _buff, toRead);
            if (n > 0)
            {
                content->append(_buff, n);
            }
            else
            {
                if (n < 0)
                {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int FileUtil::ReadSmallFile::readToBuffer(int* size)
{
    int err = _err;  
    if (_fd >= 0)
    {
        ssize_t n = ::pread(_fd, _buff, sizeof(_buff) - 1, 0);
        if (n > 0)
        {
            if (size)
            {
                *size = static_cast<int>(n);
            }
            _buff[n] = '\0';
        }
        else
        {
            err = errno; 
        }
    }

    return err;
}

FileUtil::AppendFile::AppendFile(string filename)
        : _fp(::fopen(filename.c_str(), "ae")), _writtenBytes(0)
{
    BLING_ASSERT(_fp);
    ::setbuffer(_fp, _buff, sizeof _buff);
}

FileUtil::AppendFile::~AppendFile()
{
    ::fclose(_fp);
}

void FileUtil::AppendFile::append(const char* s, const size_t len)
{
    size_t n = write(s, len);
    size_t remain = len - n;

    while (remain > 0)
    {
        size_t x = write(s + n, remain);
        if (x == 0)
        {
            int err = ferror(_fp);
            if (err)
            {
                LOG_SYSERR << "AppendFile::append() faild" << strerror_tl(err);
            }
            break;
        }
        else if ( static_cast<int>(x) < 0 )
        {
            LOG_SYSERR << "AppendFile::append() faild -1";
            break;
        }

        n += x;
        remain = len - n;
    }

    _writtenBytes += len;
}

void FileUtil::AppendFile::flush()
{
    ::fflush(_fp);
}

size_t FileUtil::AppendFile::write(const char* s, size_t len)
{
    return ::fwrite_unlocked(s, 1, len, _fp);
}















