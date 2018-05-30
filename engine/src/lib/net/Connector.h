/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-29 10:39
 * Last modified : 2018-05-29 10:39
 * Filename      : Connector.h

 * Description   : 客户端连接器
 * *******************************************************/
#ifndef _BLING_CONNECTOR_H_
#define _BLING_CONNECTOR_H_

#include "common/noncopyable.h"
#include "net/InetAddress.h"
#include "net/Socket.h"

#include <functional>
#include <memory>

namespace bling
{

namespace net 
{

class Channel;
class EventLoop;

class Connector : bling::noncopyable, 
                  public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void (std::unique_ptr<Socket>)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { _newConnectionCallback = cb; }

    void start();
    void restart(); //must be called in loop
    void stop();

    const InetAddress& serverAddress() const { return _serverAddr; }

private:
    enum States { kDisconnected, kConnecting, kConnected };
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) { _state = s; }
    void startInLoop();
    void stopInLoop();

    void connect();
    void connecting(int sockfd);
    
    void handleWrite();
    void handleError();
    
    void retry(int sockfd);
    int  removeAndResetChannel();
    void resetChannel();

    EventLoop* _loop;
    InetAddress _serverAddr;
    bool _connect;
    States _state;
    std::unique_ptr<Channel> _channel;
    NewConnectionCallback _newConnectionCallback;
    int _retryDelayMs;
}; // class Connector

}; // ns net

}; //ns bling


#endif // _BLING_CONNECTOR_H_
