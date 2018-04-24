/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-23 16:06
 * Last modified : 2018-04-23 16:06
 * Filename      : FileUtil.h

 * Description   : 读写文件操作
 * *******************************************************/
#ifndef _BLING_FILEUTIL_H_
#define _BLING_FILEUTIL_H_

#include "common/noncopyable.h"
#include "common/Type.h"

namespace bling
{

namespace FileUtil 
{

class ReadSmallFile : bling::noncopyable
{
public:
    ReadSmallFile(string filename);
    ~ReadSmallFile();

    int readToString(int maxSize, string* content, int64_t* fileSize, int64_t* modifyTime, int64_t* createTime);

    int readToBuffer(int* size);

    const char* buffer() const { return _buff; }

    static const int kBufferSize = 64*1024;

private:
    int _fd;
    int _err;
    char _buff[kBufferSize];
}; // class ReadSmallFile

/** 直接读取文件内容 **/
int readFile(string filename, int maxSize, string* content, int64_t* fileSize = NULL, int64_t* modifyTime = NULL, int64_t* createTime = NULL);

class AppendFile : bling::noncopyable
{
public:
    AppendFile(string filename);
    ~AppendFile();

    void append(const char* str, const size_t len);

    void flush();

    off_t writtenBytes() const { return _writtenBytes; }

private:
    size_t write(const char* str, size_t len);

    FILE* _fp;
    char _buff[64*1024];
    off_t _writtenBytes;
}; // class AppendFile


}; // ns FileUtil

}; //ns bling


#endif // _BLING_FILEUTIL_H_
