#include "net/TcpConnection.h"

#include "common/WeakCallback.h"
#include "log/Logger.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Socket.h"
#include "net/SocketAPI.h"

#include <errno.h>
#include <functional>

using namespace bling;
using namespace bling::net;


void bling::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    LOG_TRACE << NET_LOG_SIGN <<  conn->localAddress().toIpPort() << "->"
              << conn->peerAddress().toIpPort() << " is "
              << (conn->connected() ? "UP" : "DOWN");
}

void bling::net::defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp)
{
    LOG_TRACE << NET_LOG_SIGN << "read message data size = %d" << buf->readableBytes();
    buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg, std::unique_ptr<Socket>& socketPtr, const InetAddress& localAddr, const InetAddress&peerAddr)
    : _loop(CHECK_NOTNULL(loop)),
      _name(nameArg),
      _state(kConnecting),
      _reading(true),
      _socket(std::move(socketPtr)),
      _channel(new Channel(loop, _socket->fd())),
      _localAddr(localAddr),
      _peerAddr(peerAddr),
      _hightWaterMark(64*1024*1024)
{
    _channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    LOG_DEBUG << NET_LOG_SIGN << "TcpConnection::ctor[" << _name << "] at " << this << " fd=" << _socket->fd();
    _socket->setKeepAlive(true);
    setTcpNoDelay(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << NET_LOG_SIGN << "TcpConnection::dtor[" << _name << "] at " << this << " fd=" << _channel->fd() << " state=" << stateToString();

    BLING_ASSERT(_state == kDisconnected);
} 

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const
{
    return _socket->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const
{
    char buf[1024] ;
    buf[0] = '\0';
    _socket->getTcpInfoString(buf, sizeof buf);
    return buf;
}

void TcpConnection::send(const void* data, int len)
{
    if (_state == kConnected)
    {
        if (_loop->isInLoopThread())
        {
            sendInLoop(data, len);
        }
        else
        {
            // _loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, string(data, len))); 
            _loop->runInLoop(std::bind((void(TcpConnection::*)(string&))&TcpConnection::sendInLoop, this, string(static_cast<const char*>(data), len))); 
        }
    }    
}

void TcpConnection::send(Buffer* buf)
{
    if (_state == kConnected)
    {
        if (_loop->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        }
        else
        {
            _loop->runInLoop(std::bind((void(TcpConnection::*)(string&))&TcpConnection::sendInLoop, this, buf->retrieveAllAsString()));
        }
    } 
}

void TcpConnection::sendInLoop(string& message)
{
    sendInLoop(message.c_str(), message.length());
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    _loop->assertInLoopThread();     
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (_state == kDisconnected)
    {
        LOG_WARN << NET_LOG_SIGN << "disconnected, give up writing";
        return;
    }

    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0)
    {
        nwrote = sockets::write(_channel->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && _writeCompleteCallback)
            {
                _loop->queueInLoop(std::bind(_writeCompleteCallback, shared_from_this())); 
            }
        }
        else
        {
            nwrote = 0; 
            if (errno != EWOULDBLOCK && errno != EAGAIN)
            {
                LOG_SYSERR << NET_LOG_SIGN << "TcpConnection::sendInLoop";
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }
    
    BLING_ASSERT(remaining <= len);
    if (!faultError && remaining > 0)
    {
        size_t oldLen = _outputBuffer.readableBytes();
        if (oldLen + remaining >= _hightWaterMark && oldLen < _hightWaterMark && _hightWaterMarkCallback)
        {
            _loop->queueInLoop(std::bind(_hightWaterMarkCallback, shared_from_this(), oldLen + remaining));
            _outputBuffer.append(static_cast<const char*>(data) + nwrote, remaining);
            if (!_channel->isWriting())
            {
                _channel->enableWriteing();
            }
        }
    }
}

void TcpConnection::shutdown()
{
    if (_state == kConnected)
    {
        setState(kDisconnecting);
        _loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    } 
}

void TcpConnection::shutdownInLoop()
{
    _loop->assertInLoopThread();
    if (!_channel->isWriting())
    {
        _socket->shutdownWrite();
    }
}

void TcpConnection::forceClose()
{
    if (_state == kConnected || _state == kDisconnecting)
    {
        setState(kDisconnecting);
        _loop->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    } 
}

void TcpConnection::forceCloseWithDelay(double seconds)
{
    if (_state == kConnected || _state == kDisconnecting)
    {
        setState(kDisconnecting);
        _loop->runAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::forceClose));
    }    
}

void TcpConnection::forceCloseInLoop()
{
    _loop->assertInLoopThread();
    if (_state == kConnected || _state == kDisconnecting)
    {
        handleClose();
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
    _socket->setTcpNoDelay(on) ;
}

void TcpConnection::startRead()
{
    _loop->runInLoop(std::bind(&TcpConnection::startReadInLoop, this)); 
}

void TcpConnection::startReadInLoop()
{
    _loop->assertInLoopThread();
    if (!_reading || !_channel->isReading())
    {
        _channel->enableReading();    
        _reading = true;
    }
}

void TcpConnection::stopRead()
{
    _loop->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop()
{
    _loop->assertInLoopThread();
    if (_reading || _channel->isReading())
    {
        _channel->disableReading();
        _reading = false;
    }
}

void TcpConnection::connectEstablished()
{
    _loop->assertInLoopThread();
    BLING_ASSERT(_state == kConnecting);
    setState(kConnected);
    _channel->tie(shared_from_this());
    _channel->enableReading();

    _connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    _loop->assertInLoopThread();
    if (_state == kConnected)
    {
        setState(kDisconnected);
        _channel->disableAll();

        _connectionCallback(shared_from_this());
    }

    _channel->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
   _loop->assertInLoopThread();
   int savedErrno = 0;
   ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
   if (n > 0)
   {
       _messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
   }
   else if (n == 0)
   {
       handleClose();
   }
   else 
   {
       errno = savedErrno;
       LOG_SYSERR << NET_LOG_SIGN << "TcpConnection::handleRead";
       handleError();
   }
}

void TcpConnection::handleWrite()
{
    _loop->assertInLoopThread(); 
    if (_channel->isWriting())
    {
        ssize_t n = sockets::write(_channel->fd(), _outputBuffer.peek(), _outputBuffer.readableBytes());
        
        if (n > 0)
        {
            _outputBuffer.retrieve(n);    
            if (_outputBuffer.readableBytes() == 0)
            {
                _channel->disableWriteing();
                if (_writeCompleteCallback)
                {
                    _loop->queueInLoop(std::bind(_writeCompleteCallback, shared_from_this()));
                }

                if (_state == kDisconnecting)
                {
                    shutdownInLoop();
                }
            }
        }
        else 
        {
            LOG_SYSERR << NET_LOG_SIGN << "TcpConnection::handleWrite";
        }
    }
    else 
    {
        LOG_TRACE << NET_LOG_SIGN << "Connection fd = " << _channel->fd() << "is down, no more writing";
    }
}
void TcpConnection::handleClose()
{
    _loop->assertInLoopThread(); 
    LOG_TRACE << NET_LOG_SIGN << "fd = " << _channel->fd() << " state = " << stateToString();
    assert(_state == kConnected || _state == kDisconnecting);
    setState(kDisconnected);
    _channel->disableAll();
    TcpConnectionPtr guardThis(shared_from_this());
    _connectionCallback(guardThis);

    //必须放在最后
    _closeCallback(guardThis);
}
void TcpConnection::handleError()
{
    int err = sockets::getSocketError(_channel->fd());
    LOG_ERROR << NET_LOG_SIGN << "TcpConnection::handleError [" << _name << "] - SO_ERROR = " << err << " " << strerror_tl(err);
    handleClose();
}

const char* TcpConnection::stateToString() const
{
    switch (_state)
    {
        case kConnected:
            return "kConnected";
        case kConnecting:
            return "kConnecting";
        case kDisconnected:
            return "kDisconnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

