#include "net/Channel.h"
#include "log/Logger.h"
#include "net/EventLoop.h"

#include <sstream>
#include <sys/epoll.h>

using namespace bling;
using namespace bling::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int ffd)
    : _loop(loop),
      _fd(ffd),
      _events(0),
      _revents(0),
      _index(-1),
      _tied(false),
      _eventHandling(false),
      _addedToLoop(false)
{
}

Channel::~Channel()
{
    BLING_ASSERT(!_eventHandling);
    BLING_ASSERT(!_addedToLoop);
    if (_loop->isInLoopThread())
    {
        BLING_ASSERT(!_loop->hasChannel(this));
    }
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    _tie = obj;
    _tied = true;
}

void Channel::update()
{
    _addedToLoop = true;
    _loop->updateChannel(this);
}

void Channel::remove()
{
    BLING_ASSERT(isNoneEvent());
    _addedToLoop = false;
    _loop->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (_tied)
    {
        guard = _tie.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
   _eventHandling = true; 
   LOG_TRACE << reventsToString();
   if ((_revents & EPOLLHUP) && !(_revents & EPOLLIN))
   {
        if (_closeCallback) _closeCallback(); 
   }

   if (_revents & EPOLLERR)
   {
        if (_errorCallback) _errorCallback();
   }

   if (_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
   {
        if (_readCallback) _readCallback(receiveTime);
   }

   if (_revents & EPOLLOUT)
   {
       if (_writeCallback) _writeCallback();
   }

   _eventHandling = false;
}

string Channel::reventsToString() const
{
    return eventsToString(_fd, _revents);
}

string Channel::eventsToString() const
{
    return eventsToString(_fd, _events);
}

string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & EPOLLIN)
        oss << "IN ";
    if (ev & EPOLLPRI)
        oss << "PRI ";
    if (ev & EPOLLOUT)
        oss << "OUT ";
    if (ev & EPOLLHUP)
        oss << "HUP ";
    if (ev & EPOLLRDHUP)
        oss << "RDHUP ";
    if (ev & EPOLLERR)
        oss << "ERR ";

    return oss.str().c_str();
}





















