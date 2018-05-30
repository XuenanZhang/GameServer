/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-10 13:57
 * Last modified : 2018-05-10 13:57
 * Filename      : Channel.h

 * Description   : 分发IO事件回调，只属于一个线程，只负责一个文件描述
 * *******************************************************/
#ifndef _BLING_CHANNEL_H_
#define _BLING_CHANNEL_H_

#include "common/noncopyable.h"
#include "common/Timestamp.h"

#include <functional>
#include <memory>

namespace bling
{

namespace net 
{

class EventLoop;

class Channel : bling::noncopyable
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void setReadCallback(const ReadEventCallback& cb) { _readCallback = cb; }
    void setWriteCallback(const EventCallback& cb) { _writeCallback = cb; }
    void setCloseCallback(const EventCallback& cb) { _closeCallback = cb; }
    void setErrorCallback(const EventCallback& cb) { _errorCallback = cb; }

    void handleEvent(Timestamp receiveTime);

    /** 将share_ptr与channel关联起来, 防止在handleEvent函数处理中被销毁 **/
    void tie(const std::shared_ptr<void>&);

    int fd() const { return _fd; }
    int events() const { return _events; }
    void set_revents(int revt) { _revents = revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    void enableReading() { _events |= kReadEvent; update(); }
    void disableReading() { _events &= ~kReadEvent; update(); }
    void enableWriting() { _events |= kWriteEvent; update(); }
    void disableWriting() { _events &= ~kWriteEvent; update(); }
    void disableAll() { _events = kNoneEvent; update(); }
    bool isWriting() const { return _events & kWriteEvent; }
    bool isReading() const { return _events & kReadEvent; }

    int index() { return _index; }
    void set_index(int idx) { _index = idx; }

    string reventsToString() const;
    string eventsToString() const;

    EventLoop* ownerLoop() {  return _loop; }
    void remove();

private:
    static string eventsToString(int fd, int ev);

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* _loop;
    const int _fd;
    int _events;
    int _revents;
    int _index;

    std::weak_ptr<void> _tie;
    bool _tied;
    bool _eventHandling;
    bool _addedToLoop;
    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _closeCallback;
    EventCallback _errorCallback;

}; // class Channel

}; // ns net

}; //ns bling


#endif // _BLING_CHANNEL_H_
