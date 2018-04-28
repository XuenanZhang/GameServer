#include "net/InetAddress.h"
#include "net/Endian.h"
#include "net/SocketAPI.h"

#include <strings.h>
#include <stddef.h>

using namespace bling;
using namespace bling::net;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6), "InetAddress != sockaddr_in6");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "offsetof(sockaddr_in, sin_family) != 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "offsetof(sockaddr_in6, sin6_family) != 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "offsetof(sockaddr_in, sin_port) != 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "offsetof(sockaddr_in6, sin6_port) != 2");

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    static_assert(offsetof(InetAddress, _addr) == 0, "offsetof(InetAddress, _addr) != 0");
    static_assert(offsetof(InetAddress, _addr6) == 0, "offsetof(InetAddress, _addr6) != 0");

    if (ipv6)
    {
        bzero(&_addr6, sizeof _addr6);
        _addr6.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        _addr6.sin6_addr = ip;
        _addr6.sin6_port = sockets::hostToNet16(port);
    }
    else
    {
        bzero(&_addr, sizeof _addr);
        _addr.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
        _addr.sin_addr.s_addr = sockets::hostToNet32(ip);
        _addr.sin_port  = sockets::hostToNet16(port);
    }
}

InetAddress::InetAddress(string ip, uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        bzero(&_addr6, sizeof _addr6);
        sockets::fromIpPort(ip.c_str(), port, &_addr6);
    }
    else
    {
        bzero(&_addr, sizeof _addr);
        sockets::fromIpPort(ip.c_str(), port, &_addr);
    }
}

string InetAddress::toIp() const
{
    char buf[64] = "";    
    sockets::toIp(buf, sizeof buf, getSockAddr());
    return buf;
}

string InetAddress::toIpPort() const
{
    char buf[64] = "";    
    sockets::toIpPort(buf, sizeof buf, getSockAddr());
    return buf;
}

uint16_t InetAddress::toPort() const
{
    return sockets::netToHost16(portNetEndian());
}

uint32_t InetAddress::ipNetEndian() const
{
    BLING_ASSERT(family() == AF_INET);
    return _addr.sin_addr.s_addr;
}
