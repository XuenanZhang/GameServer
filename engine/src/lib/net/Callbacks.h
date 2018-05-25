/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-08 10:48
 * Last modified : 2018-05-08 10:48
 * Filename      : Callbacks.h

 * Description   : 回调函数类型
 * *******************************************************/
#ifndef _BLING_CALLBACKS_H_
#define _BLING_CALLBACKS_H_

#include "common/Timestamp.h"

#include <functional>
#include <memory>

namespace bling
{

namespace net 
{

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HightWaterMarkCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

typedef std::function<void()> TimerCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

}; // ns net

}; //ns bling


#endif // _BLING_CALLBACKS_H_
