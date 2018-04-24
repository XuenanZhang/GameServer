/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-24 14:50
 * Last modified : 2018-04-24 14:50
 * Filename      : ProcessInfo.h

 * Description   : 进程信息
 * *******************************************************/
#ifndef _BLING_PROCESSINFO_H_
#define _BLING_PROCESSINFO_H_

#include "common/Type.h"
#include "common/Timestamp.h"

namespace bling
{

namespace ProcessInfo 
{
//进程id
pid_t pid();
string pidString();

//用户id
uid_t uid();
string username();
uid_t euid();

Timestamp startTime();
int clockTicksPerSecond();
int pageSize();

string hostname();
string procname();

string procStatus();
string procStat();
string threadStat();
string exePath();

int openedFiles();
int maxOpenFiles();

struct CpuTime
{
    double userSeconds;
    double systemSeconds;

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) {}
};
CpuTime cpuTime();

int numThreads();
std::vector<pid_t> threads();

}; // ns ProcessInfo

}; //ns bling


#endif // _BLING_PROCESSINFO_H_
