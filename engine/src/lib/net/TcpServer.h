/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-25 18:08
 * Last modified : 2018-05-25 18:08
 * Filename      : TcpServer.h

 * Description   : 多线程服务器封装
 * *******************************************************/
#ifndef _BLING_TCPSERVER_H_
#define _BLING_TCPSERVER_H_

#include "common/noncopyable.h"
#include "common/Type.h"
#include "net/TcpConnection.h"

#include <memory>
#include <functional>
#include <unordered_map>
#include <atomic>

namespace bling
{

namespace net 
{

class Acceptor;
class Eventloop;
class EventLoopThreadPool;

class TcpServer : bling::noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg, Option option = kNoReusePort);
    ~TcpServer();

    const string& ipPort() const { return _ipPort; }
    const string& name() const { return _name; }
    EventLoop* getLoop() const { return _loop; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb) { _threadInitCallback = cb; } //not thread safe
    std::shared_ptr<EventLoopThreadPool> threadPool() { return _threadPool;}

    /** 启动服务器 **/
    void start();

    /** 新连接 或 断开连接 回调 not thread safe **/
    void setConnectionCallback( const ConnectionCallback& cb) { _connectionCallback = cb; }

    /** 接受数据回调 not thread safe **/
    void setMessageCallback(const MessageCallback& cb) { _messageCallback = cb; }

    /** 数据写入完成回调 not thread safe **/
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { _writeCompleteCallback = cb; }

private:
    // not thread safe, but in loop
    void newConnection(std::unique_ptr<Socket> socket, const InetAddress& peerAddr);
    // thread safe
    void removeConnection(const TcpConnectionPtr& conn);
    // not thread safe, but in loop
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef std::unordered_map<string, TcpConnectionPtr> ConnectionMap;
    ConnectionMap _connections;

    EventLoop* _loop;
    const string _ipPort;
    const string _name;
    std::unique_ptr<Acceptor> _acceptor;
    std::shared_ptr<EventLoopThreadPool> _threadPool;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
    ThreadInitCallback _threadInitCallback;
    std::atomic<int32_t> _started;
    int32_t _nextConnId;
    
}; // class TcpServer

}; // ns net

}; //ns bling


#endif // _BLING_TCPSERVER_H_
