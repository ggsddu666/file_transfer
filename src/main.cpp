#include <iostream>
// #include "TimerHeap.h"
#include <vector>
#include "str_tool.h"
#include <unistd.h>



bool comp(const int s, const int v) {
    return s < v;
}

template <typename T>
void display(std::vector<T> vec) {
    for(auto i : vec)
        std::cout << i << " ";
}

void test() {
    int a = 10, b = 5;
    if(comp(b, a)) {
        std::cout << "true";
    }
}




void display_timers(std::vector<Entry> timers) {
    TimeStamp timestamp(0);
    for(auto it = timers.begin(); it != timers.end(); ++it) {
        timestamp = it->first;
        std::cout << timestamp.get_microseconds() << std::endl;
    }
}

void display_timerIds(std::vector<int> timerIds) {
    for(auto it = timerIds.begin(); it != timerIds.end(); ++it) {
        std::cout << *it << std::endl;
    }
}

auto timer_handler = []	{ std::cout << "空闲断开" << std::endl;	};	//空闲断开
//int main_TimerHeap() {
//    TimerHeap timer;
//    int id1 = timer.addTimer(10000000,timer_handler);
////    T.test();
//    // test();
//
////    while (1) {
////        int id = timer.addTimer(10000000,timer_handler);
////        sleep(1);
////        timer.cancle(id);
////    }
////    int id2 = timer.addTimer(5000000,timer_handler);
////    int id3 = timer.addTimer(1000000,timer_handler);
////   // int id4 = timer.addTimer(7000000,timer_handler);
////   // int id5 = timer.addTimer(6000000,timer_handler);
////    // std::cout << id1 << " " << id2 << " " << id3 << " " << id4 << " " << id5 << std::endl;
////    display_timerIds(TimerHeap::getTimerIds());
////    display_timers(TimerHeap::getTimers());
////    sleep(5);
////    display_timers(TimerHeap::getTimers());
////    while(1) {}
//    return 0;
//}

int main {

};

