/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-05-08 10:25
 * Last modified : 2018-05-08 10:25
 * Filename      : TimerId.h

 * Description   : 
 * *******************************************************/
#ifndef _BLING_TIMERID_H_
#define _BLING_TIMERID_H_

#include "common/copyable.h"
#include "common/Type.h"

namespace bling
{

namespace net 
{

class Timer;

class TimerId : public bling::copyable
{
public:
    TimerId() : _timer(NULL), _sequence(0)
    {
    }
    ~TimerId(){};

    TimerId(Timer* timer, int64_t seq)
        : _timer(timer), _sequence(seq)
    {
    }

    friend class TimerQueue;

private:
    Timer* _timer;
    int64_t _sequence;
}; // class TimerId

}; // ns net


}; //ns bling


#endif // _BLING_TIMERID_H_
