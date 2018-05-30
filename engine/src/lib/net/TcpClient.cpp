#include "net/TcpClient.h"

#include "log/Logger.h"
#include "net/Connector.h"
#include "net/EventLoop.h"
#include "net/SocketAPI.h"

#include <stdio.h>

using namespace bling;
using namespace bling::net;


namespace bling 
{

namespace net 
{

namespace detail 
{

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{
    
}

}; // ns detail

}; // ns net

}; // ns bling

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg)
    : _loop(CHECK_NOTNULL(loop)),
      _connector(new Connector(loop, serverAddr)),
      _name(nameArg),
      _connectionCallback(defaultConnectionCallback),
      _messageCallback(defaultMessageCallback),
      _retry(false),
      _connect(true),
      _nextConnId(1)
{
    _connector->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));

    LOG_INFO << NET_LOG_SIGN << "TcpClient::TcpClient[" << _name << "] - connector " << _connector.get();
}

TcpClient::~TcpClient()
{
    LOG_INFO << NET_LOG_SIGN << "TcpClient::~TcpClient[" << _name << "] - connector " << _connector.get();
    TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockAuto lock(_mutex);
        unique = _connection.use_count() == 1 ? true : false;
        conn = _connection;
    }

    if (conn)
    {
        BLING_ASSERT(_loop == conn->getLoop());
        CloseCallback cb = std::bind(&detail::removeConnection, _loop, std::placeholders::_1);
        _loop->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));

        if (unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        _connector->stop(); 
        _loop->runAfter(1, std::bind(&detail::removeConnector, _connector));
    }
}

void TcpClient::connect()
{
    LOG_INFO << NET_LOG_SIGN << "TcpClient::connect[" << _name << "] - connecting to " << _connector->serverAddress().toIpPort();
    _connect = true;
    _connector->start();
}

void TcpClient::disconnect()
{
    _connect = false;
    
    {
        MutexLockAuto lock(_mutex);
        if (_connection)
        {
            _connection->shutdown();
        }
    }
}

void TcpClient::stop()
{
    _connect = false;
    _connector->stop();
}

void TcpClient::newConnection(std::unique_ptr<Socket> socket)
{
    _loop->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(socket->fd()));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), _nextConnId);
    ++_nextConnId;
    string connName = _name + buf;

    InetAddress localAddr(sockets::getLocalAddr(socket->fd()));

    TcpConnectionPtr conn(new TcpConnection(_loop, connName, socket, localAddr, peerAddr));
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this , std::placeholders::_1));

    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    _loop->assertInLoopThread();
    BLING_ASSERT(_loop == conn->getLoop());
    
    {
        MutexLockAuto lock(_mutex);
        BLING_ASSERT(_connection == conn);
        _connection.reset();
    }

    _loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (_retry && _connect)
    {
        LOG_INFO << NET_LOG_SIGN << "TcpClient::connect[" << _name << "] - Reconnecting to" << _connector->serverAddress().toIpPort();
        _connector->restart();
    }
}





