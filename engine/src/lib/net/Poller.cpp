#include "net/Poller.h"
#include "net/EpollPoller.h"
#include "net/Channel.h"
#include "net/EventLoop.h"

using namespace bling;
using namespace bling::net;

Poller::Poller(EventLoop * loop)
    : _ownerLoop(loop)
{
}

Poller::~Poller()
{
}

bool Poller::hasChannel(Channel* channel) const
{
    assertInLoopThread();
    ChannelMap::const_iterator it = _channels.find(channel->fd());
    return it != _channels.end() && it->second == channel;
}

void Poller::assertInLoopThread() const
{
    _ownerLoop->assertInLoopThread();
}

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    // if (::getenv("BLING_USE_EPOLL"))
   return new EpollPoller(loop);
}
