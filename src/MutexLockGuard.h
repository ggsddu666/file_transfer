//
// Created by wu on 2018/6/4.
//

#ifndef SPIDER_MUTEXLOCKGUARD_H
#define SPIDER_MUTEXLOCKGUARD_H

#include "MutexLock.h"
// 栈上对象，封装线程互斥锁的加锁和解锁（使用MutexLock类）

class MutexLockGuard {
public:
    MutexLockGuard(MutexLock &mutex):mutex(mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }

private:
    MutexLock &mutex;
};


#endif //SPIDER_MUTEXLOCKGUARD_H
