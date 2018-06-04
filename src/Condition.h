//
// Created by wu on 2018/6/4.
//

#ifndef SPIDER_CONDITION_H
#define SPIDER_CONDITION_H

// 封装条件变量

#include "MutexLock.h"

class Condition {
public:
    Condition(MutexLock &mutex): mutex_(mutex) { pthread_cond_init(&cond, nullptr); }
    ~Condition() { pthread_cond_destroy(&cond); }
    void wait() { pthread_cond_wait(&cond, mutex_.getMutex()); }
    void signal() { pthread_cond_signal(&cond); }
private:
    // 这里mutex是引用
    MutexLock &mutex_;
    pthread_cond_t cond;
};


#endif //SPIDER_CONDITION_H
