#include "net/TcpServer.h"
#include "log/Logger.h"
#include "thread/CurrentThread.h"
#include "common/ProcessInfo.h"
#include "net/EventLoop.h"
#include "net/SocketAPI.h"
#include "thread/Thread.h"
#include "net/Socket.h"
#include "common/Utils.h"

#include <unistd.h>

using namespace bling;
using namespace bling::net;

int numThreads = 0;

class EchoServer 
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr)
        : _loop(loop),
          _server(loop, listenAddr, "EchoServer")
    {
        _server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        _server.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _server.setWriteCompleteCallback(std::bind(&EchoServer::onWriteComplete, this, std::placeholders::_1));
        _server.setThreadNum(3);
    }

    void start() { _server.start();}

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " is " << (conn->connected() ? "connect" : "close"); 
        LOG_INFO << conn->getTcpInfoString();
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString()); 
        LOG_INFO << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();

        conn->send(msg.c_str(), msg.length());
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->name() << " write complete ! ";
    }

private:
    EventLoop *_loop;
    TcpServer _server;
}; // class EchoServer

void ThreadFun()
{
   while (true)
   {
        int t = utils::random(Timestamp::kMicroSecondPerSecond * 0.2, Timestamp::kMicroSecondPerSecond * 0.5);
        CurrentThread::sleepUsec(t);
   }
}

void init()
{
    LOG_INFO << "init";
    CountDownLatch cd(1);
    // sockets::createNonblocking(AF_INET6);
    // Socket s(999);
}


int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << ProcessInfo::pid() << ", tid = " << CurrentThread::tid(); 
    if (argc > 1)
    {
        numThreads = atoi(argv[1]);
    }
    bool ipv6 = argc > 2;
    EventLoop loop;
    InetAddress listenAddr(2000, false, ipv6);
    EchoServer server(&loop, listenAddr);
    server.start();

    // Thread thread(ThreadFun, "writeThread");
    // thread.start();
    
    loop.loop();
}
