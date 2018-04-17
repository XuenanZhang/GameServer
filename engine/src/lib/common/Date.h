/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-13 16:05
 * Last modified : 2018-04-13 16:05
 * Filename      : Date.h

 * Description   : 日期（本地）
 * *******************************************************/
#ifndef _BLING_DATE_H_
#define _BLING_DATE_H_

#include "common/copyable.h"

#include <time.h>

namespace bling
{

class Date : public bling::copyable
{
public:
    Date();
    // UTC 秒
    explicit Date(int secondsArg);

public:
    int year()     { return _tm.tm_year + 1900; }
    int month()    { return _tm.tm_mon + 1; }
    int day()      { return _tm.tm_mday; }
    int hour()     { return _tm.tm_hour; }
    int minutes()  { return _tm.tm_min; }
    int seconds()  { return _tm.tm_sec; };
    int week()     { return _tm.tm_wday ? _tm.tm_wday : 7; };
    int totalDay() { return _tm.tm_yday; };

private:
    time_t _seconds;
    struct tm _tm;
};

}; //bling


#endif //_BLING_DATE_H_
