#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "thread/Thread.h"
#include "thread/ThreadPool.h"
#include "common/Mutex.h"
#include "common/CountDownLatch.h"
#include "common/Timestamp.h"
#include "common/BlockingQueue.h"
#include "common/BoundedBlockingQueue.h"
#include "log/Logger.h"
#include "common/FileUtil.h"
#include "log/LogFile.h"
#include "common/ProcessInfo.h"
#include "log/AsyncLogger.h"

using namespace bling;

MutexLock mutex;
int num = 0;
void threadFun()
{
    MutexLockAuto lock(mutex);
    num++;
    printf("threadId=%d threadName=%s ==> num = %d\n", CurrentThread::tid(), CurrentThread::name(), num);
}
void test1();
void testThread()
{
    {
        CountDownLatch latch(1);
        // Thread thread1(threadFun);
        // thread1.start();
        ThreadPool tp("threadPool");
        tp.setMaxQueueSize(10);
        tp.start(10);
        for (int32_t i = 0; i < 100; ++i) 
        {   
            tp.run(test1);
        }   

        usleep(1000*1000*2);
        tp.stop();
    }
    printf("end thread.................\n");
}

std::vector<int> vv;
bling::BlockingQueue<int> bq;
int testNum = 0;
void test1()
{
    // usleep(10000);
    MutexLockAuto lock(mutex);
    int i = testNum++;
    bq.put(i);    
    printf("put --> %d\n", i);
}
void test2()
{
    int i = 0;    
    while (i < 99)
    {
        i = bq.take();    
        printf("take --> %d\n", i);
        vv.push_back(i);
    }

    printf("vv.size = %d\n", (int)vv.size());
}

void logOutput(const char* msg, int len)
{
    printf("====> %s", msg);
}
void testLog()
{
    int i = -1234;
    double d = 2345.76474754452;
    bool b = false;
    bool b1 = true;
    LOG_TRACE << "shuzi is " << i << "..";
    LOG_DEBUG << "shuzi is " << d << "..";
    LOG_INFO << "shuzi is " << b << "..";
    LOG_WARN << "shuzi is " << b1 << "..";
    LOG_ERROR << "shuzi is " << 123 << "..";
    // LOG_FATAL << "shuzi is " << 123 << "..";
    LOG_SYSERR << "shuzi is " << 123 << "..";
    int * p = NULL;
    // *p = 3;
    CHECK_NOTNULL(p);
    LOG_SYSFATAL << "shuzi is " << 123 << "..";
}

class Test
{
public:
    Test():val(10), str("abab"){}
    ~Test()
    {
        printf("destroy");
    };
    Test(const Test& t)
    {
        printf("copy construct\n") ;
    }
    Test(const Test&& t)
    {
        printf("move construct\n") ;
        val = t.val;
        str = std::move(t.str);
    }

    int val;
    string str;

};

bling::LogFile g_file("testLog", 1024*10);
bling::AsyncLogger g_AsynLog("AsynLog", 1024*10, 1);
void outf(const char* msg, int len)
{
    g_AsynLog.append(msg, len);
}

void flushf()
{
   g_file.flush(); 
}

void testLogFile()
{
    bling::Logger::setOutput(outf);
    bling::Logger::setFlush(flushf);
    string str = "1234567890 abc";
    while (true)
    {
        usleep(1000*1000);
        LOG_DEBUG << str;
        LOG_WARN << str;
        g_file.flush();
    }
}
void testProcess();
void testAsynLog()
{
    bling::Logger::setOutput(outf);
    g_AsynLog.start();
    string str = "1234567890 abc";

    testProcess();
    while (true)
    {
        usleep(1000*500);
        LOG_DEBUG << str;
        LOG_INFO << str;
        LOG_WARN << str;
    }
}

void testProcess()
{
    LOG_INFO <<  "pid = " << static_cast<int>(ProcessInfo::pid());
    LOG_INFO <<  "pidString = " << ProcessInfo::pidString();
    LOG_INFO <<  "uid = " << static_cast<int>(ProcessInfo::uid());
    LOG_INFO <<  "username = " << ProcessInfo::username();
    LOG_INFO <<  "startTime = " << ProcessInfo::startTime().getSeconds();
    LOG_INFO <<  "clockTicksPerSecond = " << ProcessInfo::clockTicksPerSecond();
    LOG_INFO <<  "pageSize = " << ProcessInfo::pageSize();
    LOG_INFO <<  "hostname = " << ProcessInfo::hostname();
    LOG_INFO <<  "procname = " << ProcessInfo::procname();
    // LOG_INFO <<  "procStatus = " << ProcessInfo::procStatus();
    // LOG_INFO <<  "procStat = " << ProcessInfo::procStat();
    // LOG_INFO <<  "threadStat = " << ProcessInfo::threadStat();
    LOG_INFO <<  "exePath = " << ProcessInfo::exePath();
    LOG_INFO <<  "openedFiles = " << ProcessInfo::openedFiles();
    LOG_INFO <<  "maxOpenFiles = " << ProcessInfo::maxOpenFiles();
    LOG_INFO <<  "numThreads = " << ProcessInfo::numThreads();
    LOG_INFO <<  "cpuTime = " << ProcessInfo::cpuTime().userSeconds << "  " << ProcessInfo::CpuTime().systemSeconds;
    
}

int main()
{
    // std::vector<std::unique_ptr<Test>> vec;
    // vec.reserve(16);
    // std::unique_ptr<Test> a(new Test);
    // std::unique_ptr<Test> b(new Test);
    // a->val = 1; 
    // a->str = "a";
    // b->val = 2; 
    // b->str = "b";
    // vec.push_back(std::move(a));
    // vec.push_back(std::move(b));
    // a = std::move(b);
    // b = std::move(vec[vec.size() - 1]);
    // b.reset(vec[0].release());
    // vec.resize(0);
    printf("aaaaa");
    
    testAsynLog();
    // testLogFile();
    // testProcess();
    // bling::Logger::setOutput(logOutput);
    // testLog();
    // bling::Thread t1(testThread, "block1");
    // bling::Thread t2(test2, "block2");
    // t1.start();
    // t2.start();

    // t1.join();
    // t2.join();

    // testThread();
    
    // printf("vv.size = %d\n", (int)vv.size());

    // printf("%s %d over....", __FILE__, __LINE__);
    return 0;
}
