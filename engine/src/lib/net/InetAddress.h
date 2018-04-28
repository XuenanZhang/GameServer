/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-27 15:29
 * Last modified : 2018-04-27 15:29
 * Filename      : InetAddress.h

 * Description   : ipv4 ipv6通用套接字地址
 * *******************************************************/
#ifndef _BLING_INETADDRESS_H_
#define _BLING_INETADDRESS_H_

#include "common/copyable.h"
#include "common/Type.h"

#include <netinet/in.h>

namespace bling
{

namespace net 
{

namespace sockets 
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
}; // ns sockets

class InetAddress : public bling::copyable
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

    InetAddress(string ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr)
        : _addr(addr)
    { }

    explicit InetAddress(const struct sockaddr_in6& addr)
        : _addr6(addr)
    { }

    sa_family_t family() const { return _addr.sin_family; }
    string toIp() const;
    string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&_addr6); }
    void setSockAddrInet6(const struct sockaddr_in6& addr6) { _addr6 = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return _addr.sin_port; }

private:
    union
    {
        struct sockaddr_in _addr;
        struct sockaddr_in6 _addr6;
    };
}; // class InetAddress

}; // ns net

}; //ns bling


#endif // _BLING_INETADDRESS_H_
