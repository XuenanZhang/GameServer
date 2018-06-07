/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-27 14:54
 * Last modified : 2018-04-27 14:54
 * Filename      : SocketAPI.h

 * Description   : socket操作封装
 * *******************************************************/
#ifndef _BLING_SOCKETAPI_H_
#define _BLING_SOCKETAPI_H_

#include <arpa/inet.h>

namespace bling
{

namespace net 
{

namespace sockets
{

//创建分阻塞socket fd
int createNonblocking(sa_family_t family);

int connect(int sockfd, const struct sockaddr* addr);
void bind(int sockfd, const struct sockaddr* addr);
void listen(int sockfd);
int accept(int sockfd, struct sockaddr_in6* addr);

ssize_t read(int sockfd, void* buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void* buf, size_t count);

void close(int sockfd);
void shutdownWrite(int sockfd);

void toIp(char* buf, size_t size, const struct sockaddr* addr);
void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);

int getSocketError(int sockfd);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

/** 获取sockfd连接上的本地地址 **/
struct sockaddr_in6 getLocalAddr(int sockfd);
/** 获取sockfd连接上的对端地址 **/
struct sockaddr_in6 getPeerAddr(int sockfd);

bool isSelfConnect(int sockfd);

}; // ns sockets

}; // ns net

}; //ns bling


#endif // _BLING_SOCKETAPI_H_
