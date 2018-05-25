#include "net/Acceptor.h"
#include "log/Logger.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/SocketAPI.cpp"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


using namespace bling;
using namespace bling::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
    : _loop(loop),
      _acceptSocket(sockets::createNonblocking(listenAddr.family())),
      _acceptChannel(loop, _acceptSocket.fd()),
      _listenning(false),
      _idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(_idleFd > 0);    
    _acceptSocket.setResuseAddr(true);
    _acceptSocket.setResusePort(reuseport);
    _acceptSocket.bindAddress(listenAddr);
}

Acceptor::~Acceptor()
{
    _acceptChannel.disableAll();
    _acceptChannel.remove();
    ::close(_idleFd);
}

void Acceptor::listen()
{
    _loop->assertInLoopThread();
    _listenning = true;
    _acceptSocket.listen();
    _acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    _loop->assertInLoopThread();
    InetAddress peerAddr;
    int connfd = _acceptSocket.accpet(&peerAddr);
    if (connfd > 0)
    {
        if (_newConnectionCallback)
        {
            // std::unique_ptr<Socket> ptr(new Socket(connfd));
            _newConnectionCallback(std::unique_ptr<Socket>(new Socket(connfd)), peerAddr);
            // _newConnectionCallback(ptr, peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
    }
    else
    {
        LOG_SYSERR << NET_LOG_SIGN << "in Acceptor::handleRead";

        if (errno == EMFILE)
        {
            ::close(_idleFd);
            _idleFd = ::accept(_acceptSocket.fd(), NULL, NULL);
            ::close(_idleFd);
            _idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}




























