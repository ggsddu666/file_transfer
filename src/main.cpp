#include <iostream>
#include "TimerHeap.h"
#include <vector>
#include "str_tool.h"
#include <unistd.h>
#include "sysutil.h"
#include "common.h"
using namespace sysutil;

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
//int main() {
//    std::string str = "qazwsxedc";
//    std::string left = "", right = "";
//    char c = 'w';
//    str_split(str, left, right, c);
//    std::cout << left << "~~" << right << std::endl;
//
//    std::string str2 = "   adasddas    ";
//    str_delspace(str2);
//    std::cout << str2 << std::endl;
//
//    std::string str3 = "qazqaz";
//    str_upper(str3);
//    std::cout << str3 << std::endl;
//
//    std::string str4 = "1622213132";
//    unsigned int re = str_octal_to_uint(str4);
//    std::cout << re << std::endl;
//
//    std::string str5 = "1622213132";
//    long long re5 = str_to_longlong(str5);
//    std::cout << re5 << std::endl;
//};

int main() {
    // 服务端
    int sock = tcp_server("127.0.0.1", 50000);

    pid_t childPid;
    switch (childPid = fork()) {
        case 0:
            // 子进程
            sleep(3);
            std::cout << "child is running" << std::endl;
            tcp_client(50000);
        default:
            int clientFd = accept(sock, NULL, 0);
            // 接受数据
            char buf[100];
            recv(clientFd, buf, sizeof(buf), 0);
            std::cout << "server recv" << std::endl;
            std::cout << buf << std::endl;
            send(clientFd, buf, sizeof(buf), 0);
    }

}

