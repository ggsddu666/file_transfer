//
// Created by wu on 2018/6/17.
//

#include <cstring>
#include "str_tool.h"


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
void str_split(std::string str, char *left, char *right, char c) {

}



