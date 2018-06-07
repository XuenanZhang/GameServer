#ifndef _BLING_UTILS_H_
#define _BLING_UTILS_H_


namespace bling
{

namespace utils 
{

/** 随机一个在[min-max]的整数 **/
int random(int min, int max);
/** 检测概率 **/
bool checkOdds(const int up, const int down);
/** 检测百分比概率 **/
inline bool checkPercent(const int val) { return checkOdds(val, 100); };
/** 检测千分比概率 **/
inline bool checkThousandth(const int val) { return checkOdds(val, 1000); };
/** 检测万分比概率 **/
inline bool checkTenThousandth(const int val) { return checkOdds(val, 10000); };

}; // ns utils

}; //ns bling


#endif // _BLING_UTILS_H_
