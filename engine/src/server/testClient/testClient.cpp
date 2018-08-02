#include "net/TcpClient.h"
#include "net/EventLoop.h"
#include "log/Logger.h"
#include "common/ProcessInfo.h"
#include "thread/Thread.h"
#include "net/SocketAPI.h"

using namespace bling;
using namespace bling::net;

class EchoClient;
int current = 0;
std::vector<std::unique_ptr<EchoClient>> clients;
class EchoClient : noncopyable
{
public:
    EchoClient(EventLoop* loop, const InetAddress& connectAddr, const string& id)
        : _loop(loop),
          _client(loop, connectAddr, "EchoClient" + id)
    {
        _client.setConnectionCallback(std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
        _client.setMessageCallback(std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        // _client.enableRetry();
    }

    void connect()
    {
        _client.connect();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> " << conn->peerAddress().toIpPort() << " is " << (conn->connected() ? "connect" : "close");

        if (conn->connected())
        {
            ++current;
            if (static_cast<size_t>(current) < clients.size())
            {
                clients[current]->connect();
            }
            LOG_INFO << " start connected id = " << current;
            
            conn->send("hello world");
        }
        else
            conn->send("byebye world");


    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString()); 
        LOG_INFO << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
        
        conn->send(std::move(msg));
    }

private:
    EventLoop* _loop;
    TcpClient _client;
}; // class EchoClient

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

    EventLoop loop;
    bool ipv6 = argc >= 3;
    InetAddress serverAddr(argc >= 2 ? argv[1] : "127.0.0.1", 2000, ipv6);

    int n = argc >=1 ? atoi(argv[0]) : 1;
    clients.reserve(n);

    for (int32_t i = 0; i < n; ++i) 
    {   
        char buf[32];
        snprintf(buf, sizeof buf, "%d", i + 1);
        clients.emplace_back(new EchoClient(&loop, serverAddr, buf));
    }   

    clients[current]->connect();
    loop.loop();

}
