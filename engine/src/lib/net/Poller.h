/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-10 14:06
 * Last modified : 2018-05-10 14:06
 * Filename      : Poller.h

 * Description   : 网络IO模型基类
 * *******************************************************/
#ifndef _BLING_POLLER_H_
#define _BLING_POLLER_H_

#include "common/noncopyable.h"
#include "net/Channel.h"
#include "common/Timestamp.h"

#include <vector>
#include <map>

namespace bling
{

namespace net 
{

class Channel;
class EventLoop;

class Poller : bling::noncopyable
{
public:
    typedef std::vector<Channel*> ChannelVector;
    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelVector* activeChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop * loop);

    void assertInLoopThread() const;

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap _channels;

private:
    EventLoop* _ownerLoop;
}; // class Poller

}; // ns net

}; //ns bling


#endif // _BLING_POLLER_H_
