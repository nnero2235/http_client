//
// Created by NNERO on 15/12/6.
//

#include "http.h"
#include "d_error.h"
#include "util.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

char *mime_type[] = {
        "text/html",
        "text/xml",
        "text/plain",
        "application/pdf",
        "application/json",
        "application/x-gzip",
        "image/jpeg",
        "image/png",
        "video/mpeg"
};

char *char_set[] = {
        "gbk",
        "utf-8"
};

char *language[] ={
        "zh-CN,zh"
};

char *connection_state[] ={
        "keep-alive",
        "close"
};

char *request_header_names[]={
        "Date",
        "Accept",
        "Accept-Language",
        "Connection",
        "Host",
        "User-Agent",
        "Accept-Encoding",
        "Accept-Charset"
};

char *response_header_names[]={
      "Server",
      "Date",
      "Content-Type",
      "Content-Length"
};

char* get_current_time(){
    time_t time_long = time(NULL);
    char *time = ctime(&time_long);
    time[strlen(time)-1] = '\0'; //去掉换行符
    return time;
}





