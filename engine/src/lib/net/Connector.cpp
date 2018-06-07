#include "net/Connector.h"

#include "log/Logger.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/SocketAPI.h"

#include <errno.h>

using namespace bling;
using namespace bling::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : _loop(loop),
      _serverAddr(serverAddr),
      _connect(false),
      _state(kDisconnected),
      _retryDelayMs(kInitRetryDelayMs)
{
}

Connector::~Connector()
{
   BLING_ASSERT(!_channel); 
}

void Connector::start()
{
    _connect = true;
    _loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    _loop->assertInLoopThread();
    BLING_ASSERT(_state == kDisconnected);
    if (_connect)
    {
        connect();
    }
    else
    {
        LOG_DEBUG << NET_LOG_SIGN << "do not connect";
    }
}

void Connector::stop()
{
    _connect = false;
    _loop->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
    _loop->assertInLoopThread();
    if (_state == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect()
{
    int sockfd = sockets::createNonblocking(_serverAddr.family());
    int ret = sockets::connect(sockfd, _serverAddr.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;

    switch (savedErrno)
    {
        case 0:
        case EINPROGRESS: //操作正在执行
        case EINTR:       //系统调用被阻止
        case EISCONN:     //传输终端已经连接
            connecting(sockfd);
            break;
    
        case EAGAIN:        //资源暂时不可用
        case EADDRINUSE:    //地址已经被使用
        case EADDRNOTAVAIL: //不能解析请求的地址
        case ECONNREFUSED:  //连接被拒绝
        case ENETUNREACH:   //网络不可达
            retry(sockfd);
            break;

        case EACCES: //没有许可权限
        case EPERM: //操作不允许
        case EAFNOSUPPORT: //Address family不支持的协议
        case EALREADY: //操作已经在执行
        case EBADF: //错误的文件描述符
        case EFAULT: //错误的地址
        case ENOTSOCK: //Socket操作在一个非socket上
            LOG_SYSERR << NET_LOG_SIGN << "connect error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;

        default:
            LOG_SYSERR << NET_LOG_SIGN << "Unexpected error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;
    }
}

void Connector::restart()
{
    _loop->assertInLoopThread();    
    setState(kDisconnected);
    _retryDelayMs = kInitRetryDelayMs;
    _connect = true;
    startInLoop();
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    BLING_ASSERT(!_channel);
    _channel.reset(new Channel(_loop, sockfd));
    _channel->setWriteCallback(std::bind(&Connector::handleWrite, this));
    _channel->setErrorCallback(std::bind(&Connector::handleError, this));

    _channel->enableWriting();
}

int Connector::removeAndResetChannel()
{
    _channel->disableAll();
    _channel->remove();
    int sockfd = _channel->fd();
    //必须在回调里重置，因为可能在Channel::handleEvent里
    _loop->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel()
{
    _channel.reset();
}

void Connector::handleWrite()
{
    LOG_TRACE << NET_LOG_SIGN << "Connector::handleWrite " << _state; 

    if (_state == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err)
        {
            LOG_WARN << NET_LOG_SIGN << "Connector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(err);
            retry(sockfd);
        }
        else if (sockets::isSelfConnect(sockfd))
        {
            LOG_WARN << NET_LOG_SIGN << "Connector::handleWrite - Self connect";
            retry(sockfd);
        }
        else 
        {
            setState(kConnected);
            if (_connect)
            {
                _newConnectionCallback(std::unique_ptr<Socket>(new Socket(sockfd)));
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
    else
    {
        BLING_ASSERT(_state == kDisconnected);
    }
}

void Connector::handleError()
{
    LOG_ERROR << NET_LOG_SIGN << "Connector::handleError state = " << _state;
    if (_state == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE << NET_LOG_SIGN << "SO_ERROR = " << err << " " << strerror_tl(err);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);
    if (_connect)
    {
        LOG_INFO << NET_LOG_SIGN << "Connector::retry - Retry connecting to " << _serverAddr.toIpPort() << " in " << _retryDelayMs << " milliseconds";
        _loop->runAfter(_retryDelayMs / 1000.0, std::bind(&Connector::startInLoop, shared_from_this()));
        _retryDelayMs = std::min(_retryDelayMs * 2, kMaxRetryDelayMs);
    }
    else
    {
        LOG_DEBUG << NET_LOG_SIGN << "do not connect";
    }
}











