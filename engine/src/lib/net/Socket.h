#ifndef _BLING_SOCKET_H_
#define _BLING_SOCKET_H_

#include "common/noncopyable.h"

struct tcp_info;
namespace bling
{

namespace net 
{

class InetAddress;

class Socket : bling::noncopyable
{
public:
    explicit Socket(int sockfd)
        : _sockfd(sockfd)
    { }

    ~Socket();

    int fd() const { return _sockfd; }

    bool getTcpInfo(struct tcp_info* tcpi) const;
    bool getTcpInfoString(char* buf, int len) const;

    void bindAddress(const InetAddress& localaddr);

    void listen();

    int accpet(InetAddress* peeraddr);

    void shutdownWrite();

    /**
     * IPPROTO_TCP级别，开启关闭Nagle算法(减少小分组数)
     */
    void setTcpNoDelay(bool on);

    /**
     * SOL_SOCKET级别，处于TIME_WAIT状态下的socket可以重复绑定使用(防止重启方无法bind)
     */
    void setRevAndSendBuf(int size);

    /**
     * SOL_SOCKET级别，处于TIME_WAIT状态下的socket可以重复绑定使用(防止重启方无法bind)
     */
    void setResuseAddr(bool on);

    /**
     * SOL_SOCKET级别，完全重复捆绑
     */
    void setResusePort(bool on);

    /**
     * SOL_SOCKET级别, 保持连接检测对方主机是否崩溃
     */
    void setKeepAlive(bool on);

private:
    const int _sockfd;
}; // class Socket

}; // ns net

}; //ns bling


#endif // _BLING_SOCKET_H_
