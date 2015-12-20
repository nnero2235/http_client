//
// Created by nnero on 15/12/8.
//

#include <string.h>

#ifndef HTTP_CLIENT_UTIL_H
#define HTTP_CLIENT_UTIL_H

/*复制src 到dest  返回当前src的index*/
size_t strncpy_index(char *dest,size_t index,const char *src,size_t len);

/*用symbol字符串分割字符串*/
char ** split_str_by_string(const char *str,const char *symbol);

/*用字符分割字符串 */
char ** split_str_by_char(const char *str,const char symbol);

/*用字符分割字符串 n表示分割次数 最多为n次*/
char ** split_n_str_by_char(const char *str, const char symbol, unsigned int n);

/*检查url是否合法 返回host path*/
char ** check_url(const char *url);

/*通过url得到端口号*/
int get_port(const char *url);

#endif //HTTP_CLIENT_UTIL_H
