#include "net/EpollPoller.h"
#include "log/Logger.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace bling;
using namespace bling::net;

// EPOLL_CTL_ADD：      注册新的fd到epfd中；
// EPOLL_CTL_MOD：      修改已经注册的fd的监听事件；
// EPOLL_CTL_DEL：      从epfd中删除一个fd；

namespace
{
const int kNew     = -1;    //不在_channels中并且不在poller事件中
const int kAdded   = 1;   //已在_channels中并且已在poller事件中
const int kDeleted = 2; //已在_channels中并且不在poller事件中
}

EpollPoller::EpollPoller(EventLoop* loop)
    : Poller(loop),
      _epollfd(::epoll_create1(EPOLL_CLOEXEC)),
      _events(kInitEventListSize)
{
    if (_epollfd < 0)
    {
        LOG_SYSFATAL << NET_LOG_SIGN << "epoll_create1 failed";
    }
}

EpollPoller::~EpollPoller()
{
    ::close(_epollfd);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelVector* activeChannels)
{
    LOG_TRACE << NET_LOG_SIGN << "fd total count " << _channels.size();

    int numEvents = ::epoll_wait(_epollfd, &*_events.begin(), static_cast<int>(_events.size()), timeoutMs);

    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        LOG_TRACE << NET_LOG_SIGN << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == _events.size())
        {
            _events.resize(_events.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_TRACE << NET_LOG_SIGN << "nothing happened";
    }
    else 
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_SYSERR << NET_LOG_SIGN << "EpollPoller::poll()";
        }
    }

    return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelVector* activeChannels) const
{
    BLING_ASSERT(static_cast<size_t>(numEvents) <= _events.size());
    for (int i = 0; i < numEvents; ++i) 
    {
        Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
        int fd = channel->fd();
        ChannelMap::const_iterator it = _channels.find(fd);
        BLING_ASSERT(it != _channels.end());
        BLING_ASSERT(it->second == channel);

        channel->set_revents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::updateChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    const int index = channel->index();
    LOG_TRACE << NET_LOG_SIGN << "fd = " << channel->fd() << " events = " << channel->events() << " index = " << index;

    int fd = channel->fd();
    if (index == kNew || index == kDeleted)
    {
        if (index == kNew)
        {
            BLING_ASSERT(_channels.find(fd) == _channels.end());
            _channels[fd] = channel;
        }
        else 
        {
            BLING_ASSERT(_channels.find(fd) != _channels.end());
            BLING_ASSERT(_channels[fd] == channel);
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else 
    {
        BLING_ASSERT(_channels.find(fd) != _channels.end());
        BLING_ASSERT(_channels[fd] == channel);
        BLING_ASSERT(index == kAdded);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE << NET_LOG_SIGN << "fd = " << fd;
    BLING_ASSERT(_channels.find(fd) != _channels.end());
    BLING_ASSERT(_channels[fd] == channel);
    BLING_ASSERT(channel->isNoneEvent());
    int index = channel->index();
    BLING_ASSERT(index == kAdded || index == kDeleted);
    size_t n = _channels.erase(fd);
    (void)n;
    BLING_ASSERT(n == 1);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}

void EpollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_TRACE << NET_LOG_SIGN << "epoll_ctl op = " << operationToString(operation) << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
    if (::epoll_ctl(_epollfd, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_SYSERR << NET_LOG_SIGN << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
        }
        else 
        {
            LOG_SYSFATAL << NET_LOG_SIGN << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
        }
    }
}

const char* EpollPoller::operationToString(int op)
{
    switch (op)
    {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            BLING_ASSERT(false && "ERROR op");
            return "Unknown Operation";
    }
}
