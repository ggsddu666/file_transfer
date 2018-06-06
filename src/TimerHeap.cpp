//
// Created by wu on 2018/6/5.
//


#include <iostream>
#include "TimerHeap.h"


/***********堆的相关操作*************/
typedef std::vector<Entry>::iterator Iterator;
typedef std::vector<int>::iterator Iterator_int;  // 仅做测试用

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
    while(holeIndex > 0 && value < *(begin + parent)) {
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


TimerHeap::TimerHeap() {
//    pthread_t thread;
//    pthread_create(&thread, NULL, &loop_timer, NULL);
}

TimerHeap::~TimerHeap() {
//     close(timerFd);
}

inline bool comp(const int s, const int v) {
    return s < v;
}

void TimerHeap::test() {
//    std::vector<int> vec = {1, 3, 5, 11, 4, 6, 7, 12 ,15, 10, 9, 8, 2};
//    push_heap(vec.begin(), vec.end(), comp);

    // 此时用的是系统的pop_heap，如果要用自定义pop_heap，将pop_heap函数的参数Iterator改为Iterator_int
//    std::vector<int> vec = {0, 1, 7, 2, 5, 9, 10, 6, 3};
//    pop_heap(vec.begin(), vec.end(), comp);
//    for(auto i : vec)
//       std::cout << i << " ";
}
