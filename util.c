//
// Created by apple on 15/12/8.
//

#include "util.h"
#include <string.h>
#include <stdlib.h>

#define MAX_STRS 128

char *strs_buffer[MAX_STRS];

int get_port(char *url){ // :8080/xxdee 这样的字符串
    for(int i=0;i<7;i++){ //最多只要判断7次 因为端口最大65535
        if(url[i] == '/'){
            char *port = malloc((i)* sizeof(char)); //\0 needed
            strncpy(port,&url[1],i-1);
            port[i-1] = '\0';
            return atoi(port);
        }
    }
    return 80; //没有写端口 默认80端口
}

size_t strncpy_index(char *src,size_t index,char *desc,size_t len){
    if(desc == NULL){
        return index;
    }
    strncpy(&src[index],desc,len);
    return index+len;
}

char ** split_str_by_string(char *str,char *symbol){
    int index = 0;
    int start = 0;
    int end = 0;
    size_t sym_len = strlen(symbol);
    size_t len = strlen(str);
    char *temp = strstr(str,symbol);
    while (temp){
        end = (int)(temp-str);
        if(end == start) { //跳过 相邻的 相同symbol
            start = end + sym_len;
            temp = strstr(&str[start],symbol);
            continue;
        }
        char *s = malloc((end-start+1)* sizeof(char)); //char 1个字节
        strncpy(s,&str[start],(end-start));
        s[end-start] = '\0';
        strs_buffer[index++] = s;
        start = end+sym_len;
        temp = strstr(&str[start],symbol);
    };
    //追加最后一个字符串
    char *s = malloc((len-start+1) * sizeof(char));
    strncpy(s,&str[start],(len-start));
    s[len-start] = '\0';
    strs_buffer[index++] = s;

    char **strs = malloc(index * sizeof(char *));
    for(int i=0;i<index;i++){
        strs[i] = strs_buffer[i];
    }
    strs[index] = NULL;
    return strs;
}

char ** split_str_by_char(char *str,char symbol){
    int index = 0;
    int start = 0;
    int end = 0;
    size_t len = strlen(str);
    for(int i=0;i<len;i++){
        if(str[i] == symbol){
            end = i;
            char *s = malloc((end-start) * sizeof(char));
            strncpy(s,&str[start],(end-start));
            strs_buffer[index++] = s;
            start = i+1;
        }
    }
    //追加最后一个字符串
    char *s = malloc((len-start) * sizeof(char));
    strncpy(s,&str[start],(len-start));
    strs_buffer[index++] = s;

    char **strs = malloc(index * sizeof(char *));
    for(int i=0;i<index;i++){
        strs[i] = strs_buffer[i];
    }
    strs[index] = NULL;
    return strs;
}

//检查url是否合法,合法返回 host  不合法返回null
char** check_url(char *url){
    size_t len = strlen(url);
    char **temp = malloc(2* sizeof(char *));
    if(len > 7 && strstr(url,"http://")) { //http://开头才是合法的
        for (int i = 7; i < len; i++) {
            if(url[i] == '/'){ // split host
                temp[0] = malloc((i-6)* sizeof(char)); // \0 needed
                strncpy(temp[0],&url[7],(i-7));
                temp[0][i-7] = '\0';

                temp[1] = malloc((len-i+1) * sizeof(char));
                strncpy(temp[1],&url[i],(len-i));
                temp[1][len-i] = '\0';
            } else {
                temp[0] = malloc((len-6)* sizeof(char));
                strncpy(temp[0],&url[7],(len-7));
                temp[0][len-7] ='\0';

                temp[1] = malloc(2);
                temp[1][0] = '/';
                temp[1][1] = '\0';
            }
        }
        return temp;
    } else {
        return NULL;
    }
}
