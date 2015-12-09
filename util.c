//
// Created by apple on 15/12/8.
//

#include "util.h"
#include <string.h>
#include <stdlib.h>

#define MAX_STRS 128

char *strs_buffer[MAX_STRS];

size_t strncpy_index(char *src,size_t index,char *desc,size_t len){
    strncpy(&src[index],desc,len);
    return index+len;
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
