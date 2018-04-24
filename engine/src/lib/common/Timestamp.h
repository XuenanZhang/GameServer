/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-12 11:56
 * Last modified : 2018-04-12 11:56
 * Filename      : Timestamp.h

 * Description   : 时间戳 
 * *******************************************************/
#ifndef _BLING_TIMESTAMP_H_
#define _BLING_TIMESTAMP_H_

#include "common/copyable.h"
#include "common/Type.h"

#include <utility>
#include <vector>

namespace bling
{

class Timestamp : public bling::copyable 
{
public:
    Timestamp() : _microSeconds(0)
    {
    }

    explicit Timestamp(int64_t  micorSeconds) : _microSeconds(micorSeconds)
    {
    }

    void swap(Timestamp& other) { std::swap(_microSeconds, other._microSeconds);}

    string toString();
    string toFormatString(bool showMicorSeconds = true);

    bool valid() const { return _microSeconds > 0; }

    int64_t getMircoSeconds() { return _microSeconds; }

    time_t getSeconds() { return static_cast<time_t>(_microSeconds / kMicroSecondPerSecond); }

    void addTime(double seconds)
    {
        int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondPerSecond);
        _microSeconds += delta;
    }

    const Timestamp & operator+ (const Timestamp &rt)
    {
        _microSeconds += rt._microSeconds;
        return *this;
    }
    const Timestamp & operator- (const Timestamp &rt)
    {
        if (_microSeconds < rt._microSeconds)
            _microSeconds = 0;
        else
            _microSeconds -= rt._microSeconds;
        return *this;
    }
    bool operator > (const Timestamp &rt) const
    {
        return _microSeconds > rt._microSeconds;
    }
    bool operator >= (const Timestamp &rt) const
    {
        return _microSeconds >= rt._microSeconds;
    }
    bool operator < (const Timestamp &rt) const
    {
        return _microSeconds < rt._microSeconds;
    }
    bool operator <= (const Timestamp &rt) const
    {
        return _microSeconds <= rt._microSeconds;
    }
    bool operator == (const Timestamp &rt) const
    {
        return _microSeconds == rt._microSeconds;
    }

public:
    static Timestamp now();
    static Timestamp invalid() { return Timestamp(); }
    static Timestamp fromUnixTime(time_t t) { return fromUnixTime(t,0); }
    static Timestamp fromUnixTime(time_t t, int microseconds) { return Timestamp(static_cast<int64_t>(t) * kMicroSecondPerSecond + microseconds); }

    static const int kMicroSecondPerSecond = 1000 * 1000;

private:
    int64_t _microSeconds;
};

// inline bool operator<(Timestamp& lhs, Timestamp& rhs)
// {
    // return lhs.getMircoSeconds() < rhs.getMircoSeconds();
// }

// inline bool operator==(Timestamp& lhs, Timestamp& rhs)
// {
    // return lhs.getMircoSeconds() == rhs.getMircoSeconds();
// }

inline double timeDiff(Timestamp& hight, Timestamp& low)
{
    int64_t diff = hight.getMircoSeconds() - low.getMircoSeconds();
    return static_cast<double>(diff) / Timestamp::kMicroSecondPerSecond;
}

inline Timestamp addTime(Timestamp& timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondPerSecond);
    return Timestamp(timestamp.getMircoSeconds() + delta);
}

}; //bling


#endif //_BLING_TIMESTAMP_H_
