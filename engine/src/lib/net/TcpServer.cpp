#include "net/TcpServer.h"

#include "log/Logger.h"
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/EventLoopThreadPool.h"
#include "net/SocketAPI.h"

#include <stdio.h>
#include <functional>

using namespace bling;
using namespace bling::net;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg, Option option)
    : _loop(CHECK_NOTNULL(loop)),
      _ipPort(listenAddr.toIpPort()),
      _name(nameArg),
      _acceptor(new Acceptor(loop, listenAddr, option == kReusePort)),
      _threadPool(new EventLoopThreadPool(loop, _name)),
      _connectionCallback(defaultConnectionCallback),
      _messageCallback(defaultMessageCallback),
      _nextConnId(1)
{
    _acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    _loop->assertInLoopThread();
    LOG_TRACE << NET_LOG_SIGN << "TcpServer::~TcpServer [" << _name << "] destructing";

    for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); ++it)
    {
        TcpConnectionPtr conn(it->second);
        it->second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    BLING_ASSERT(0 <= numThreads);
    _threadPool->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if (_started.fetch_add(1) == 0)
    {
        _threadPool->start(_threadInitCallback);

        BLING_ASSERT(!_acceptor->listenning());
        _loop->runInLoop(std::bind(&Acceptor::listen, _acceptor.get()));
    }
}

void TcpServer::newConnection(std::unique_ptr<Socket> socket, const InetAddress& peerAddr)
{
    _loop->assertInLoopThread(); 
    EventLoop* ioLoop = _threadPool->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", _ipPort.c_str(), _nextConnId);
    ++_nextConnId;
    string connName = _name + buf;
    InetAddress localAddr(sockets::getLocalAddr(socket->fd()));

    LOG_INFO << NET_LOG_SIGN << "TcpServer::newConnection [" << _name << "] - new connection [" << connName << "] from " << peerAddr.toIpPort() << " localAddr = " << localAddr.toIpPort();;

    TcpConnectionPtr conn(new TcpConnection(ioLoop, connName, socket, localAddr, peerAddr));
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    _loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    _loop->assertInLoopThread();
    LOG_INFO << NET_LOG_SIGN << "TcpServer::removeConnectionInLoop [" << _name << "] - connection " << conn->name();
    size_t n = _connections.erase(conn->name());
    (void)n;
    BLING_ASSERT(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}


