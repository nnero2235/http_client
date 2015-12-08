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

char ** spilt_str_by_char(char *str,char symbol){
    int index = 0;
    size_t len = strlen(str);
    for(int i=0;i<len;i++){
        if(str[i] == symbol){
            str[i] = '\0';
            char *s = malloc(strlen(str) * sizeof(char));
            strncpy(s,str,strlen(str));
            str[i] = symbol;
            strs_buffer[index++] = s;
        }
    }
    strs_buffer[index] = NULL;
    char **strs = malloc(index * sizeof(char *));
    for(int i=0;i<index;i++){
        *strs[i] = *strs_buffer[i];
    }
    return strs;
}
