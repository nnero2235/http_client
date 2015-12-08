//
// Created by apple on 15/12/8.
//

#include <string.h>

#ifndef HTTP_CLIENT_UTIL_H
#define HTTP_CLIENT_UTIL_H

/*复制src 到dest  返回当前src的index*/
size_t strncpy_index(char *dest,size_t index,char *src,size_t len);

char ** split_str_by_string(char *str,char *symbol);

char ** split_str_by_char(char *str,char symbol);

#endif //HTTP_CLIENT_UTIL_H
