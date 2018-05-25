/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-25 11:17
 * Last modified : 2018-05-25 11:17
 * Filename      : TcpConnection.h

 * Description   : 每个已连接的socket对应一个此类封装
 * *******************************************************/
#ifndef _BLING_TCPCONNECTION_H_
#define _BLING_TCPCONNECTION_H_

#include "common/noncopyable.h"
#include "net/Buffer.h"
#include "net/InetAddress.h"
#include "net/Callbacks.h"

#include <memory>

struct tcp_info;

namespace bling
{

namespace net 
{

class Channel;
class EventLoop;
class Socket;

class TcpConnection : bling::noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop,
                  const string& name,
                  std::unique_ptr<Socket>& socketPtr,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return _loop; }
    const string& name() const { return _name; }
    const InetAddress& localAddress() const { return _localAddr; }
    const InetAddress& peerAddress() const { return _peerAddr; }
    bool connected() const { return _state == kConnected; }
    bool disconnected() const { return _state == kDisconnected; }
    bool getTcpInfo(struct tcp_info*) const;
    string getTcpInfoString() const;

    void send(const void* data, int len);
    void send(Buffer* buffer);
    
    void shutdown(); //not thread safe
    void forceClose();
    void forceCloseWithDelay(double seconds);
    
    void setTcpNoDelay(bool on);

    void startRead();
    void stopRead();
    bool isReading() const { return _reading; }

    void setContext(void * context) { _context = context; }
    void* getContext() { return _context; }

    void setConnectionCallback(const ConnectionCallback& cb) { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { _writeCompleteCallback = cb; }
    void setCloseCallback(const CloseCallback& cb) { _closeCallback = cb; }
    void setHeighWaterMarkCallback(const HightWaterMarkCallback& cb, size_t hightWaterMark)
    { _hightWaterMarkCallback = cb; _hightWaterMark = hightWaterMark; }

    Buffer* inputBuffer() { return &_inputBuffer; }
    Buffer* outputBuffer() { return &_outputBuffer; }

    /** 当TcpServer接受新连接时调用 **/
    void connectEstablished();
    /** 当TcpServer移除连接时调用 **/
    void connectDestroyed();

private:
    enum StateConnect { kConnected, kConnecting, kDisconnected, kDisconnecting };
    
    // event handle
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(string& message);
    void sendInLoop(const void* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void startReadInLoop();
    void stopReadInLoop();

    void setState(StateConnect s) { _state = s; }
    const char* stateToString() const;

private:
    EventLoop* _loop;
    const string _name;
    StateConnect _state;
    bool _reading;

    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    const InetAddress _localAddr;
    const InetAddress _peerAddr;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
    CloseCallback _closeCallback;
    HightWaterMarkCallback _hightWaterMarkCallback;
    size_t _hightWaterMark;
    Buffer _inputBuffer;
    Buffer _outputBuffer;
    void* _context;

}; // class TcpConnection


}; // ns net

}; //ns bling


#endif // _BLING_TCPCONNECTION_H_
