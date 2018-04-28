/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-28 18:01
 * Last modified : 2018-04-28 18:01
 * Filename      : Buffer.h

 * Description   : 网络buff缓冲
 * *******************************************************/
#ifndef _BLING_BUFFER_H_
#define _BLING_BUFFER_H_

#include "common/copyable.h"
#include "common/Type.h"

#include <vector>
#include <algorithm>
#include <string.h>

namespace bling
{

namespace net 
{

class Buffer : public bling::copyable
{
public:
    static const size_t kCheapPrepend = 0;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : _buffer(kCheapPrepend + initialSize),
          _readerIndex(kCheapPrepend),
          _writerIndex(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    void swap(Buffer& rhs)
    {
        _buffer.swap(rhs._buffer);
        std::swap(_readerIndex, rhs._readerIndex);
        std::swap(_writerIndex, rhs._writerIndex);
    }

    size_t readableBytes() const { return _writerIndex - _readerIndex; }
    size_t writableBytes() const { return _buffer.size() - _writerIndex; }
    size_t prependableBytes() const { return _readerIndex; }


    /** 查询数据 不增加为任何位移 **/
    const char* peek() const { return begin() + _readerIndex; }
    // sockets::netToHost ----
    int8_t peekInt8() const
    {
        BLING_ASSERT(readableBytes() >= sizeof(int8_t));
        int8_t val = *peek();
        return val;
    }
    int8_t peekInt16() const
    {
        BLING_ASSERT(readableBytes() >= sizeof(int8_t));
        int16_t val = 0;
        ::memcpy(&val, peek(), sizeof val);
        return val;
    }
    int8_t peekInt32() const
    {
        BLING_ASSERT(readableBytes() >= sizeof(int8_t));
        int32_t val = 0;
        ::memcpy(&val, peek(), sizeof val);
        return val;
    }
    int64_t peekInt64() const
    {
        BLING_ASSERT(readableBytes() >= sizeof(int8_t));
        int64_t val = 0;
        ::memcpy(&val, peek(), sizeof val);
        return val;
    }
    void peek(int8_t& val) { val = peekInt8(); };
    void peek(int16_t& val) { val = peekInt16(); };
    void peek(int32_t& val) { val = peekInt32(); };
    void peek(int64_t& val) { val = peekInt64(); };
    void peek(uint8_t& val) { val = static_cast<uint8_t>(peekInt8()); };
    void peek(uint16_t& val) { val = static_cast<uint16_t>(peekInt16()); };
    void peek(uint32_t& val) { val = static_cast<uint32_t>(peekInt32()); };
    void peek(uint64_t& val) { val = static_cast<uint64_t>(peekInt64()); };

    /** 增加reader位置，检测是否恢复读写位置 **/
    void retrieve(size_t len);
    void retrieveInt8() { return retrieve(sizeof(int8_t)); }
    void retrieveInt16() { return retrieve(sizeof(int16_t)); }
    void retrieveInt32() { return retrieve(sizeof(int32_t)); }
    void retrieveInt64() { return retrieve(sizeof(int64_t)); }
    void retrieveAll()
    {
        _readerIndex = kCheapPrepend;
        _writerIndex = kCheapPrepend;
    }
    string retrieveAsString(size_t len);
    string retrieveAllAsString() { return retrieveAsString(readableBytes()); }

    /** 读取数据,增加位移 **/
    char* beginRead() { return begin() + _readerIndex; }
    const char* beginRead() const { return begin() + _readerIndex; }
    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }
    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }
    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }
    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }
    Buffer& operator >> (int8_t& val) 
    { 
        val = readInt8();  
        return *this; 
    }
    Buffer& operator >> (int16_t& val) 
    { 
        val = readInt16();  
        return *this; 
    }
    Buffer& operator >> (int32_t& val) 
    { 
        val = readInt32();  
        return *this; 
    }
    Buffer& operator >> (int64_t& val) 
    { 
        val = readInt64();  
        return *this; 
    }
    Buffer& operator >> (uint8_t& val) 
    { 
        val = readInt8();  
        return *this; 
    }
    Buffer& operator >> (uint16_t& val) 
    { 
        val = readInt16();  
        return *this; 
    }
    Buffer& operator >> (uint32_t& val) 
    { 
        val = readInt32();  
        return *this; 
    }
    Buffer& operator >> (uint64_t& val) 
    { 
        val = readInt64();  
        return *this; 
    }

    /** 追加数据 **/
    void append(const char* data, size_t len);
    void append(const void* data, size_t len) { append(static_cast<const char*>(data), len); }
    // sockets::hostToNet ----
    void appendInt8(int8_t val) { append(&val, sizeof val); }
    void appendInt16(int16_t val) { append(&val, sizeof val); }
    void appendInt32(int32_t val) { append(&val, sizeof val); }
    void appendInt64(int64_t val) { append(&val, sizeof val); }
    Buffer& operator << (int8_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (int16_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (int32_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (int64_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (uint8_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (uint16_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (uint32_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    Buffer& operator << (uint64_t val) 
    { 
        append(&val, sizeof val);  
        return *this; 
    }
    
    
    /** 确保可写入len长度 **/
    void ensureWritableBytes(size_t len);
    char* beginWrite() { return begin() + _writerIndex; }
    const char* beginWrite() const { return begin() + _writerIndex; }
    void hasWritten(size_t len)
    {
        BLING_ASSERT(len <= writableBytes());
        _writerIndex += len;
    }
    void unwrite(size_t len)
    {
        BLING_ASSERT(len <= readableBytes());
        _writerIndex -= len;
    }

    /** 读取socket真实数据到buff **/
    ssize_t readFd(int fd, int* savedErrno);

private:
    char* begin() { return &*_buffer.begin(); }
    const char* begin() const { return &*_buffer.begin(); }

    /** 扩展空间 **/
    void makeSpace(size_t len);
private:
    std::vector<char> _buffer;
    size_t _readerIndex;
    size_t _writerIndex;
    

}; // class Buffer

}; // ns net

}; //ns bling


#endif // _BLING_BUFFER_H_
