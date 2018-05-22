#include "log/LogStream.h"

#include <algorithm>

namespace bling 
{

namespace detail 
{

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do 
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while ( i!=0 );

    if (value < 0)
    {
        *p++ = '-';
    }

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char* p = buf;

    do 
    {
        int lsd = static_cast<int>(i % 16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

// template class FixedBuff<kSmallBuffer>;
// template class FixedBuff<kBigBuffer>;

}; // ns detail

}; // ns bling

using namespace bling;

template<typename T>
void LogStream::formatInteger(T v)
{
    if (_buffer.avail() >= kMaxNumbericSize)
    {
        size_t len = detail::convert(_buffer.current(), v);
        _buffer.add(len);
    }
}

LogStream& LogStream::operator<<(bool v)
{
    // _buffer.append(v ? "1" : "0", 1);
    if (v)
    {
        _buffer.append("true", 4);
    }
    else 
    {
        _buffer.append("false", 5);
    }

    return *this;
}
LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;

}
LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(float v)
{
    *this << static_cast<double>(v);
    return *this;
}
LogStream& LogStream::operator<<(double v)
{
    if (_buffer.avail() >= kMaxNumbericSize)
    {
        int len = snprintf(_buffer.current(), kMaxNumbericSize, "%.12g", v);
        _buffer.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char v)
{
    _buffer.append(&v, 1);
    return *this;
}
LogStream& LogStream::operator<<(const char* v)
{
    if (v)
    {
        _buffer.append(v, strlen(v));
    }
    else 
    {
        _buffer.append("(null)", 6);
    }
    return *this;
}
LogStream& LogStream::operator<<(const unsigned char* v)
{
    *this << reinterpret_cast<const char*>(v);
    return *this;
}
LogStream& LogStream::operator<<(const string& v)
{
    _buffer.append(v.c_str(), v.size());
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (_buffer.avail() >= kMaxNumbericSize)
    {
        char* buf = _buffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = detail::convertHex(buf + 2, v);
        _buffer.add(len + 2);
    }
    return *this;
}
using namespace bling::detail;
LogStream& LogStream::operator<<(const Buffer& buf)
{
    *this << buf.toString();
    return *this;
}


template<typename T>
Fmt::Fmt(const char* fmt, T val)
{
    _length = snprintf(_buf, sizeof _buf, fmt, val);
    BLING_ASSERT(static_cast<size_t>(_length) < sizeof _buf);
}

//
template Fmt::Fmt(const char*, char);
template Fmt::Fmt(const char*, short);
template Fmt::Fmt(const char*, unsigned short);
template Fmt::Fmt(const char*, int);
template Fmt::Fmt(const char*, unsigned int);
template Fmt::Fmt(const char*, long);
template Fmt::Fmt(const char*, unsigned long);
template Fmt::Fmt(const char*, long long);
template Fmt::Fmt(const char*, unsigned long long);
template Fmt::Fmt(const char*, float);
template Fmt::Fmt(const char*, double);

