//
// Created by wu on 2018/6/4.
//

#ifndef SPIDER_MUTEXLOCK_H
#define SPIDER_MUTEXLOCK_H

// 使用RAII技术封装互斥锁
/*
 *
 *   RAII（Resource Acquisition Is Initialization）是一种利用对象生命周期来控制程序资源（如内存、文件句柄、网络连接、互斥量等等）的简单技术。
　　  RAII 的一般做法是这样的：在对象构造时获取资源，接着控制对资源的访问使之在对象的生命周期内始终保持有效，
   最后在对象析构的时候释放资源。借此，我们实际上把管理一份资源的责任托管给了一个对象。这种做法有两大好处：
     不需要显式地释放资源。
     采用这种方式，对象所需的资源在其生命期内始终保持有效。
 *
 *
 *
 * */

#include <pthread.h>

class MutexLock {
public:
    MutexLock() { pthread_mutex_init(&mutex, nullptr); }
    ~MutexLock() { pthread_mutex_destroy(&mutex);}
    pthread_mutex_t* getMutex()  {   return &mutex;  }
    // 加锁
    void lock() { pthread_mutex_lock(&mutex); }
    // 减锁
    void unlock() { pthread_mutex_unlock(&mutex); }
private:
    pthread_mutex_t mutex;
};


#endif //SPIDER_MUTEXLOCK_H
