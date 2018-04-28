#include "net/Socket.h"
#include "log/Logger.h"
#include "net/InetAddress.h"
#include "net/SocketAPI.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

using namespace bling;
using namespace bling::net;


Socket::~Socket()
{
    sockets::close(_sockfd);
}

bool Socket::getTcpInfo(struct tcp_info* tcpi) const
{
    socklen_t len = sizeof(*tcpi);
    bzero(tcpi, len);
    return ::getsockopt(_sockfd, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
    
}

bool Socket::getTcpInfoString(char* buf, int len) const
{
    struct tcp_info tcpi; 
    bool ok = getTcpInfo(&tcpi);

    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u rto=%u ato=%u snd_mss=%u rcv_mss=%u " 
                 "lost=%u, retrans=%u rtt=%u rttvar=%u sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                 tcpi.tcpi_rto,          // Retransmit timeout in usec 
                 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                 tcpi.tcpi_snd_mss,
                 tcpi.tcpi_rcv_mss,
                 tcpi.tcpi_lost,         // Lost packets
                 tcpi.tcpi_retrans,      // Retransmitted packets out
                 tcpi.tcpi_rtt,          // Smoothed round trip time in usec
                 tcpi.tcpi_rttvar,       // Medium deviation
                 tcpi.tcpi_snd_ssthresh,  
                 tcpi.tcpi_snd_cwnd,
                 tcpi.tcpi_total_retrans);  // Total retransmits for entire connection 
    }

    return ok;
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    sockets::bind(_sockfd, localaddr.getSockAddr()) ;
}

void Socket::listen()
{   
    sockets::listen(_sockfd);
}

int Socket::accpet(InetAddress* peeraddr)
{
    struct sockaddr_in6 addr; 
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(_sockfd, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }

    return connfd;
}

void Socket::shutdownWrite()
{
    sockets::shutdownWrite(_sockfd);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0; 
    ::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setRevAndSendBuf(int size)
{
    ::setsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &size, static_cast<socklen_t>(sizeof size));
    ::setsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &size, static_cast<socklen_t>(sizeof size));
}

void Socket::setResuseAddr(bool on)
{
    int optval = on ? 1 : 0;    
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setResusePort(bool on)
{
    int optval = on ? 1 : 0;    
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;    
    ::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}

