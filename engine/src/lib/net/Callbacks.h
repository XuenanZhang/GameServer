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

#include <functional>

namespace bling
{

namespace net 
{

typedef std::function<void()> TimerCallback;

}; // ns net

}; //ns bling


#endif // _BLING_CALLBACKS_H_
