//
// Created by wu on 2018/6/5.
//

#ifndef SPIDER_TIMESTAMP_H
#define SPIDER_TIMESTAMP_H


#include <cstdint>

class TimeStamp {
public:
    TimeStamp(int64_t microseconds_arg): microseconds(microseconds_arg) {}
    ~TimeStamp() {}
    int64_t get_microseconds() { return microseconds; }

private:
    int64_t microseconds;
};

inline bool operator<(TimeStamp &s, TimeStamp &h) { return s.get_microseconds() < h.get_microseconds(); }
inline bool operator>(TimeStamp &s, TimeStamp &h) { return s.get_microseconds() < h.get_microseconds(); }
inline bool operator==(TimeStamp &s, TimeStamp &h) { return s.get_microseconds() == h.get_microseconds(); }

#endif //SPIDER_TIMESTAMP_H
