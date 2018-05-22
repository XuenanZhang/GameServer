/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-18 17:24
 * Last modified : 2018-05-18 17:24
 * Filename      : Acceptor.h

 * Description   : 链接接收器
 * *******************************************************/
#ifndef _BLING_ACCEPTOR_H_
#define _BLING_ACCEPTOR_H_

#include "common/noncopyable.h"
#include "net/Socket.h"
#include "net/Channel.h"

#include <functional>

namespace bling
{

namespace net 
{

class EventLoop;
class InetAddress;

class Acceptor : bling::noncopyable
{
public:
    typedef std::function<void(std::unique_ptr<Socket>, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }

    bool listenning() const { return _listenning; }

    void listen();

private:
    void handleRead(); 

    EventLoop* _loop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    NewConnectionCallback _newConnectionCallback;
    bool _listenning;
    int _idleFd;
}; // class Acceptor

}; // ns net

}; //ns bling


#endif // _BLING_ACCEPTOR_H_
