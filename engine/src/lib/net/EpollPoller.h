/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-09 10:43
 * Last modified : 2018-05-09 10:43
 * Filename      : EpollPoller.h

 * Description   : epoll模型封装
 * *******************************************************/
#ifndef _BLING_EPOLLPOLLER_H_
#define _BLING_EPOLLPOLLER_H_

#include "net/Poller.h"

#include <vector>

struct epoll_event;

namespace bling
{

namespace net 
{

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop* loop);
    virtual ~EpollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelVector* activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);

private:
    static const int kInitEventListSize = 16;

    static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelVector* activeChannels) const;

    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventVector;

    int _epollfd;
    EventVector _events;
}; // class EpollPoller

}; // ns net

}; //ns bling


#endif // _BLING_EPOLLPOLLER_H_
