//
// Created by wu on 2018/6/17.
//

#ifndef SPIDER_STR_TOOL_H
#define SPIDER_STR_TOOL_H

#include <iostream>
#include <string>

void str_trim_crlf(char *str);
void str_split(const std::string str, std::string &left, std::string &right, const char c);
void str_delspace(std::string &str);
void str_upper(std::string &str);
unsigned int str_octal_to_uint(std::string &str);
long long str_to_longlong(const std::string &str);


#endif //SPIDER_STR_TOOL_H
