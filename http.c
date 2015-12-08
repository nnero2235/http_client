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

char *language[] ={
        "zh-cn"
};

char *connection_state[] ={
        "Keep-Alive",
        "Close"
};

char *request_header_names[]={
        "Date:",
        "Accept:",
        "Accept-Language:",
        "Host:",
        "User-Agent:",
        "Accept-encoding:"
};

char *response_header_names[]={
      "Server",
      "Date",
      "Content-Type",
      "Content-Length"
};

char* get_current_time(){
    time_t time_long = time(NULL);
    return ctime(&time_long);
}





