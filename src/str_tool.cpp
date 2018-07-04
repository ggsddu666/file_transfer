//
// Created by wu on 2018/6/17.
//

#include <cstring>
#include <iostream>
#include "str_tool.h"
#include <algorithm>
#include <sstream>


/*
 *  去除换行符
 *
 * */

void str_trim_crlf(char *str) {
    char *p = &str[strlen(str) - 1];
    while(*p == '\n' || *p == '\r')
        *p-- = '\0';
}

/**
 * 分割字符串，分割后两个子串不包含c
 * @param str   要处理的字符串
 * @param left  左子字符串
 * @param right 右子字符串
 * @param c     分割标志
 */
void str_split(const std::string str, std::string &left, std::string &right, const char c) {
    auto it = str.find(c);
    if(it != std::string::npos) {
        left = str.substr(0, it);
        right = str.substr(it);
    }
}

/**
 * 去除首尾空格
 * @param  str 要处理的字符串
 * @return 返回处理后的字符串
 */
void str_delspace(std::string &str) {
    if(!str.empty()) {
        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" "));
    }
}

/**
 * 将字符串转换为大写
 * @param str 要处理的字符串
 */
void str_upper(std::string &str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/**
 * 将八进制转换成unsigned int
 * @param str 要处理的字符串
 */
unsigned int str_octal_to_uint(std::string &str) {
    bool is_first_digit = false;
    unsigned int re = 0;
    for(int i = 0; i < str.size(); ++i) {
        if(!isdigit(str[i]) || str[i] > '7') {
            break;
        }
        if(str[i] != '0') { // 找到第一个不为0的数字
            is_first_digit = true;
        }
        if(is_first_digit) {
            // re乘8
            re <<= 3;
            re += str[i] - '0';
        }
    }

    return re;
}

/**
 * 将字符串转换成long long
 * @param str 要处理的字符串
 */
long long str_to_longlong(const std::string &str) {
    std::stringstream ss;
    ss << str;
    long long re;
    ss >> re;
    return re;
}
