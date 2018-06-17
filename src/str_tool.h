//
// Created by wu on 2018/6/17.
//

#ifndef SPIDER_STR_TOOL_H
#define SPIDER_STR_TOOL_H

#include <iostream>
#include <string>

void str_trim_crlf(char *str);
void str_split(std::string str , char *left, char *right, char c);
char *str_delspace(char *str);
void str_upper(char *str);
unsigned int str_octal_to_uint(char *str);
long long str_to_longlong(const char *str);


#endif //SPIDER_STR_TOOL_H
