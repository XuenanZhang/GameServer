/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-29 17:34
 * Last modified : 2018-05-29 17:34
 * Filename      : TcpClient.h

 * Description   : Tcp客户端, 一个TcpClient带有一个connector和TcpConnection
 * *******************************************************/
#ifndef _BLING_TCPCLIENT_H_
#define _BLING_TCPCLIENT_H_

#include "common/Mutex.h"
#include "net/TcpConnection.h"

namespace bling
{

namespace net 
{

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : bling::noncopyable
{
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg);
    ~TcpClient();

    void connect();

    void disconnect();

    void stop();

    TcpConnectionPtr connection() const
    {
        MutexLockAuto lock(_mutex);
        return _connection;
    }

    EventLoop* getLoop() const { return _loop;}
    bool retry() const { return _retry; }
    void enableRetry() { _retry = true; }

    const string& name() const { return _name; }

    // not thread safe
    void setConnectionCallback(ConnectionCallback cb) { _connectionCallback = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { _messageCallback = std::move(cb); }
    void setWriteCompleteCallback(WriteCompleteCallback cb) { _writeCompleteCallback = std::move(cb); }

private:
    void newConnection(std::unique_ptr<Socket> socket);// not thread safe

    void removeConnection(const TcpConnectionPtr& conn);// not thread safe

    EventLoop* _loop;
    ConnectorPtr _connector;
    const string _name;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
    bool _retry;
    bool _connect;

    int _nextConnId;
    mutable MutexLock _mutex;
    TcpConnectionPtr _connection;
}; // class TcpClient

}; // ns net

}; //ns bling


#endif // _BLING_TCPCLIENT_H_
