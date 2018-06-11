//
// Created by wu on 2018/6/5.
//

#ifndef SPIDER_TIMERHEAP_H
#define SPIDER_TIMERHEAP_H

#include "TimeStamp.h"
#include <pthread.h>
#include <sys/timerfd.h>
#include <functional>
#include <vector>

typedef std::function<void()> TimerCallback;
// 类函数对象  TimerCallback函数

typedef std::pair<TimeStamp, TimerCallback> Entry;

class TimerHeap {
public:
    // 开启监听线程
    TimerHeap();
    ~TimerHeap();
    void test();
private:
    static int timerFd;
    static void *loop_timer(void*);
};




#endif //SPIDER_TIMERHEAP_H
