//
// Created by wu on 2018/6/5.
//


#include <iostream>
#include <string.h>
#include "TimerHeap.h"
#include "MutexLock.h"
#include "Condition.h"
#include "MutexLockGuard.h"
#include <unistd.h>
#include <sys/epoll.h>


int createTimer();




/***********堆的相关操作*************/
typedef std::vector<Entry>::iterator Iterator;
typedef std::vector<int>::iterator Iterator_int;  // 仅做测试用

inline bool comp(const int s, const int v) {
    return s < v;
}

/*
 *  加入结点
 *  其中父结点为（i-1）/2
 *
 *
 * */
template <typename Comp>
int push_heap(Iterator begin, Iterator end, Comp comp) {
    int len = end - begin;
    int holeIndex = len - 1;
    int parent = (holeIndex - 1) / 2;
    Entry value = *(end - 1);
    while(holeIndex > 0 && comp(*(begin + holeIndex),*(begin + parent))) {
        *(begin + holeIndex) = *(begin + parent);
        holeIndex = parent;
        parent = (parent - 1) / 2;
    }
    *(begin + holeIndex) = value;
    return holeIndex;
}

/*
 *  堆的调整
 *
 *
 * */

template <typename Comp>
int adjust_heap(Iterator begin, Iterator end, int holeIndex, int len, Comp comp) {
    int nextIndex = 2 * holeIndex + 1;
    while(nextIndex < len) {
        // 从nextIndex、nextIndex+1选出一个较小的值
        if(nextIndex < len - 1 && comp(*(begin + nextIndex + 1), *(begin + nextIndex)))
            ++nextIndex;
        if(comp(*(begin + holeIndex), *(begin + nextIndex)))
            break;
        std::swap(*(begin + nextIndex), *(begin + holeIndex));
        holeIndex = nextIndex;
        nextIndex = nextIndex * 2 + 1;
    }
}

/**
 *
 * 删除节点
 *
 *
 * */
template <typename Comp>
void pop_heap(Iterator begin, Iterator end, Comp comp) {
    std::swap(*(begin), *(end - 1));
    int len = end - begin - 1;
    adjust_heap(begin, end, 0, len, comp);
}

/***********堆的相关操作end*************/


// 创建个线程 调用loop_timer函数
TimerHeap::TimerHeap() {
    pthread_t thread;
    pthread_create(&thread, NULL, &loop_timer, NULL);
}



TimerHeap::~TimerHeap() {
//     close(timerFd);
}


/*********** 定时器 ************/

MutexLock mutex;
Condition cond(mutex);
int TimerHeap::timerFd = createTimer();
std::vector<Entry> TimerHeap::timers;
std::vector<int> TimerHeap::timerIds;

/*
 *
 *
 * */
timespec homMuchTimeFromNow(TimeStamp when) {
    int64_t microseconds = when.get_microseconds();
    if(microseconds < 100)
        microseconds = 100;
    struct timespec ts;
    // 秒和纳秒
    ts.tv_sec = microseconds / TimeStamp::kmicroseconds;
    ts.tv_nsec = microseconds % TimeStamp::kmicroseconds * 1000;
    return ts;
}

/*
 *
 *  创建一个timerfd
 *
 * */
int createTimer() {
    // CLOCK_MONOTONIC:以固定的速率运行，从不进行调整和复位 ,它不受任何系统time-of-day时钟修改的影响
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC );
    // 如果是阻塞状态  只有当定时器到期时候，read才能读到信息  否则就是阻塞状态
   // int timerfd = timerfd_create(CLOCK_MONOTONIC,   TFD_CLOEXEC );
    if(timerfd == -1) {
        std::cout << "timerfd_create error" << std::endl;
        exit(0);
    }
    return timerfd;
}

/**
 *
 *  设置定时器到期时间
 *
 * */
void resetTimerFd(int timerfd, TimeStamp expiration) {
    int re = 0;
    struct itimerspec oldValue;
    struct itimerspec newValue;
    bzero(&oldValue, sizeof(oldValue));
    bzero(&newValue, sizeof(newValue));
    newValue.it_value = homMuchTimeFromNow(expiration);
    newValue.it_interval = homMuchTimeFromNow(expiration);
    if(re = timerfd_settime(timerfd, 0, &newValue, &oldValue) == -1) {
        std::cout << "timerfd_settime error" << std::endl;
        exit(0);
    }
}


/**
 *
 *  往timer加入定时器
 *  addTimer后返回TimerId，用来区分定时任务，方便cancle
 *  问题1：每次进入后，其他的index可能会变
 *  问题2：为什么要重新设置时间   resetTimerFd(timerFd, timers.begin()->first);
 *
 *
 * */
int TimerHeap::addTimer(TimeStamp when, TimerCallback cb) {
    MutexLockGuard mutexLock(mutex);
    timers.push_back(Entry(when, cb));
    int index = push_heap(timers.begin(), timers.end(), EntryComp());
    // std::cout << "index:" << index << std::endl;
    // 重新设置到期时间
    resetTimerFd(timerFd, timers.begin()->first);
    // 保存在timersIds
    int tids_len = timerIds.size();
    for(int i = 0; i < tids_len; ++i) {
        if(timerIds[i] == -1) {
            timerIds[i] = index;
            std::cout << "ID:" << i << std::endl;
            return i;
        }
    }
    timerIds.push_back(index);
    std::cout << "ID:" << tids_len << std::endl;
    return tids_len;

}

/**
 *
 *  取消定时器
 *  原理：将这个定时器放在timers末尾，将其pop出，最后调整堆,然后将timerIds中此pos置为-1
 *
 *
 * */
// 问题：这里应该要调整timerFd
void TimerHeap::cancle(int timerId) {
    MutexLockGuard mutexLock(mutex);
    int index = timerIds[timerId];
    std::swap(timers[index], timers[timers.size() - 1]);
    timers.pop_back();
    adjust_heap(timers.begin(), timers.end(), index, timers.size(), EntryComp());
    timerIds[timerId] = -1;
}

/*
 *  执行定时器任务，并将已到达时间的定时器清除
 *  因为已经根据超时时间排序,因此只需监听第一个是否超时即可
 *
 *  这里
 *      pop_heap(timers.begin(), timers.end(), EntryComp());
		timers.pop_back();
 *  与cancle原理一样：
 *      std::swap(timers[index], timers[timers.size() - 1]);
        timers.pop_back();
        adjust_heap(timers.begin(), timers.end(), index, timers.size(), EntryComp());


     问题1：timers.begin()->first == now？
           timers进行pop之后，上面肯定不成立呀。。。

 *
 * */
void TimerHeap::handle_read() {
    reset();
}
void TimerHeap::reset() {
    TimeStamp now = timers.begin()->first;
    while( !timers.empty() && timers.begin()->first == now ) {
        timers.begin()->second();
        pop_heap(timers.begin(), timers.end(), EntryComp());
        timers.pop_back();
    }
    if(!timers.empty())
        resetTimerFd(timerFd, timers.begin()->first);
}


void *TimerHeap::loop_timer(void *type)
{
    int epoll_fd = epoll_create(1);
    epoll_event event;
    event.events = EPOLLIN;
    int ctl = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,timerFd,&event);
    if(ctl < 0)
        std::cout << "create epoll error" << std::endl;
    int num_events = 0;
    while(1)
    {
        if(!timers.empty())
        {
            MutexLockGuard mutexLock(mutex);
            //epoll时加入定时器使用timerfd_settime系统调用会使timerFD变成可读
            num_events = epoll_wait(epoll_fd, &event, 5, 10);	//10毫秒查一次
            if(num_events > 0)
            {
                std::cout << "epoll" << std::endl;
                handle_read();
                num_events = 0;
            }
        }
    }
}


/*********** 定时器end ************/


void TimerHeap::test() {
     /***** 测试堆操作 ***/
//    std::vector<int> vec = {1, 3, 5, 11, 4, 6, 7, 12 ,15, 10, 9, 8, 2};
//    push_heap(vec.begin(), vec.end(), comp);
    // 此时用的是系统的pop_heap，如果要用自定义pop_heap，将pop_heap函数的参数Iterator改为Iterator_int
    // std::vector<int> vec = {0, 1, 7, 2, 5, 9, 10, 6, 3};
   // pop_heap(vec.begin(), vec.end(), comp);
//    for(auto i : vec)
//       std::cout << i << " ";
     /***** 测试堆操作end ***/

//    TimeStamp T(103423444);
//    timespec ts;
//    ts = homMuchTimeFromNow(T);
//    std::cout << ts.tv_sec << " " << ts.tv_nsec << std::endl;


    /***** 测试定时器 ***/
    // 测试resetTimerFd函数
//    int numExp = 0;
//    resetTimerFd(TimerHeap::timerFd, 2000000);
//    for(; numExp < 100; ) {
//        //sleep(2);
//        read(TimerHeap::timerFd, &numExp, sizeof(uint64_t));
//        std::cout << numExp << std::endl;
//    }

    // 测试addTimer函数  cancle函数
    // addTimer(TimeStamp when, TimerCallback cb);
    // 1, 3, 5, 11, 4, 6, 7, 12 ,15, 10, 9, 8, 2
    TimeStamp t1(1000000), t2(300000), t3(500000), t4(1100000),t5(400000),t6(600000),t7(700000),t8(1200000),t9(1500000);
    TimeStamp t10(10000000), t11(9000000), t12(8000000),t13(2000000);
    TimerCallback cb;

    std::cout << addTimer(t1, cb) << std::endl;
    //sleep(2);
    std::cout << addTimer(t2, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t3, cb)<< std::endl;
    //sleep(2);
    std::cout << addTimer(t4, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t5, cb)<< std::endl;
    //sleep(2);
    std::cout << addTimer(t6, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t7, cb)<< std::endl;
    //sleep(2);
    std::cout << addTimer(t8, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t9, cb)<< std::endl;
    //sleep(2);
    std::cout << addTimer(t10, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t11, cb)<< std::endl;
    //sleep(2);
    std::cout << addTimer(t12, cb)<< std::endl;
    //sleep(2);
    std::cout<<addTimer(t13, cb)<< std::endl;


    for(auto it = timerIds.begin(); it != timerIds.end(); ++it) {
        std::cout << *it << std::endl;
    }



    /***** 测试定时器end ***/
}
