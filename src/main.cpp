#include <iostream>
#include "TimerHeap.h"
#include <vector>


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

int main() {
    TimerHeap T;
    T.test();
    // test();
    return 0;
}