/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-20 11:36
 * Last modified : 2018-04-20 11:36
 * Filename      : LogStream.h

 * Description   : 日志流 方便添加信息
 * *******************************************************/
#ifndef _BLING_LOGSTREAM_H_
#define _BLING_LOGSTREAM_H_

#include "common/noncopyable.h"
#include "common/Type.h"

#include <string.h>

namespace bling
{

namespace detail 
{

const int kSmallBuffer = 40000;
const int kBigBuffer = 40000 * 1000;

template<int SIZE>
class FixedBuff : bling::noncopyable
{
public:
    FixedBuff()
        : _cur(_data)
    {
    }
    ~FixedBuff()
    {
    };

    void append(const char* buf, size_t len)
    {
        if (static_cast<int>(avail()) > len)
        {
            memcpy(_cur, buf, len);
            _cur += len;
        }
    }

    const char* data() const { return _data; }
    int length() const { return static_cast<int>(_cur - _data); }

    char* current() { return _cur; }

    int avail() const { return static_cast<int>(end() - _cur); }

    void add(size_t len) { _cur += len; }

    void reset() { _cur = _data; }
    void bzero() { ::bzero(_data, sizeof _data); } 

    const char* debugString();

    string toString() const { return string(_data, length()); }

private:
    const char* end() const { return _data + sizeof _data;}

private:
    char _data[SIZE];
    char* _cur;
}; // class FixedBuff

}; // ns detail

class LogStream : bling::noncopyable 
{
public:
    typedef detail::FixedBuff<detail::kSmallBuffer> Buffer;

    LogStream& operator<<(bool);
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    
    LogStream& operator<<(float);
    LogStream& operator<<(double);

    LogStream& operator<<(char);
    LogStream& operator<<(const char*);
    LogStream& operator<<(const unsigned char*);
    LogStream& operator<<(const string&);

    LogStream& operator<<(const void*);

    LogStream& operator<<(const Buffer&);

    void append(const char* data, int len) { _buffer.append(data, len); }
    const Buffer& buffer() const { return _buffer; }
    void resetBuff() { _buffer.reset(); }

private:
    template<typename T>
    void formatInteger(T);

private:
    Buffer _buffer;

    static const int kMaxNumbericSize = 32;
}; // class LogStream

class Fmt 
{
public:
    template<typename T>
    Fmt(const char* fmt, T val);

    const char* data() const { return _buf; }
    int length() const { return _length; }

private:
    char _buf[32];
    int _length;
}; // class Fmt

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

}; //bling


#endif // _BLING_LOGSTREAM_H_
