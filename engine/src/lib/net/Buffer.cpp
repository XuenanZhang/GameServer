#include "net/Buffer.h"
#include "net/SocketAPI.h"

#include <sys/uio.h>
#include <errno.h>

using namespace bling;
using namespace bling::net;

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        _writerIndex += n; 
    }
    else 
    {
        _writerIndex = _buffer.size();
        append(extrabuf, n - writable);
    }

    return n;
}

void Buffer::retrieve(size_t len)
{
    BLING_ASSERT(len <= readableBytes());
    if (len < readableBytes())
    {
        _readerIndex += len;
    }
    else
    {
        retrieveAll();
    }
}

string Buffer::retrieveAsString(size_t len)
{
    BLING_ASSERT(len <= readableBytes());
    string result(peek(), len);
    retrieve(len);
    return std::move(result);
}

void Buffer::append(const char* data, size_t len)
{
    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWritten(len);
}

void Buffer::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len);
    }
    BLING_ASSERT(writableBytes() >= len);
}

void Buffer::makeSpace(size_t len)
{
    if (writableBytes() + prependableBytes() < len + kCheapPrepend)
    {
        _buffer.resize(_writerIndex + len);
    }
    else
    {
        BLING_ASSERT(kCheapPrepend < _readerIndex);
        size_t readable = readableBytes();
        std::copy(beginRead(), beginWrite(), begin() + kCheapPrepend);
        _readerIndex = kCheapPrepend;
        _writerIndex = _readerIndex + readable;
        BLING_ASSERT(readable == readableBytes());
    }  
}
