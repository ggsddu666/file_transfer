//
// Created by wu on 2018/6/5.
//

#ifndef SPIDER_TIMERHEAP_H
#define SPIDER_TIMERHEAP_H


#include <pthread.h>
//#include <sys/timerfd.h>
#include <functional>
#include <vector>
#include "TimeStamp.h"


typedef std::function<void()> TimerCallback;
// 类函数对象  TimerCallback函数

typedef std::pair<TimeStamp, TimerCallback> Entry;

struct EntryComp {
    bool operator()(const Entry &a, const Entry &b) {
        return a.first > b.first;
    }
};


//  1.实例化TimerHeap类后，创建个线程，其中线程函数为loop，故调用loop_timer，loop中epoll监听timerFd中epoll监听timerFd(最小堆中第一个fd)
//    涉及函数 createTimer、loop_timer
//  2.若定时器时间到了，会调用handle_read，handle_read再调用reset，reset会重置（把最小堆中第一个的回调函数执行完，pop出，
//    然后timerFd指向新的fd）。
//    涉及函数 handle_read、reset、resetTimerFd
//  3.若新加入定时器，则堆重新调整，timerFd指向新的fd
//    涉及函数 addTimer、resetTimerFd
//  4.若在定时器规定时间内，取消定时器，则将这个定时器pop出
//    涉及函数 cancle（这里应该还有resetTimerFd）
class TimerHeap {
public:
    // 开启监听线程
    TimerHeap();
    ~TimerHeap();
    void test();
    int addTimer(TimeStamp when, TimerCallback cb);
    void cancle(int timerId);
    static std::vector<Entry> getTimers() { return timers; }
    static std::vector<int> getTimerIds() { return timerIds; }
    static void handle_read();
    static void reset();

private:
    static int timerFd;
    static void *loop_timer(void*);
    // 存放Entry类型，按最小堆排序，通过比较每个Entry的定时器时间大小
    static std::vector<Entry> timers;
    // timerIds的pos为timerId，其内容为timers的pos，通过pos可以找到相应的定时器,如果被cancle则置为1
    // 这边timerId的值为第几个push进timers.
    static std::vector<int> timerIds;
};




#endif //SPIDER_TIMERHEAP_H
