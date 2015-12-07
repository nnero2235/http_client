//
// Created by NNERO on 15/12/6.
//

#include "http.h"
#include "d_error.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <strings.h>

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

#define REQUEST_STR_SIZE 2048

static size_t set_param_str(char *param_str,struct request_param *params){
    size_t param_len = 0;
    size_t param_index=0;
    param_str[param_index] = PARAM_SPLIT;
    param_index++;
    while(params){
        //解析name
        param_len = strlen(params->name);
        strncpy(&param_str[param_index],params->name,param_len);
        param_index += param_len;
        //添加=号
        param_str[param_index] = PARAM_EQUAL;
        param_index++;
        //解析value
        param_len = strlen(params->value);
        strncpy(&param_str[param_index],params->value,param_len);
        param_index += param_len;
        //调整指针
        params = params->next;
        if(params) {
            param_str[param_index] = PARAM_AND;
            param_index++;
        }
    }
    param_str[++param_index] = '\0';
    printf("param_str: %s\n",param_str);
    return param_index-1;
}
//GET
static size_t set_request_line(char *str,struct http_request *request){
    char param_str[BUFSIZ];
    size_t str_index=0;
    size_t len=0;

    strncpy(str,"GET/",4);
    str_index += 3; //index 从0开始

    len = strlen(request->request_line);
    strncpy(&str[str_index+1],request->request_line, len);
    str_index+=len;

    len = set_param_str(param_str,request->params);
    strncpy(&str[str_index+1],param_str,len);
    str_index += len;

    strncpy(&str[str_index+1]," ",1);
    str_index += 1;

    len = strlen(HTTP_VERSION);
    strncpy(&str[str_index+1],HTTP_VERSION,len);
    str_index += len;

    strncpy(&str[++str_index],"\n",1);
    return str_index;
}

char* get_current_time(){
    time_t time_long = time(NULL);
    return ctime(&time_long);
}

int make_client_socket(char *host,int port){
    int sock_id;
    struct sockaddr_in server_addr;
    struct hostent *hp;

    sock_id = socket(AF_INET,SOCK_STREAM,0);
    if(sock_id == -1){
        p_err("socket");
        return -1;
    }

    hp = gethostbyname(host);
    bzero(&server_addr, sizeof(struct sockaddr_in));
    bcopy(hp->h_addr,&server_addr.sin_addr,hp->h_length);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    printf("Trying connect to %s:%d\n",host,port);

    if(connect(sock_id,(struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        p_err("connect");
        return -1;
    }
    printf("connect server success.\n");
    return sock_id;
}

char * make_http_request(struct http_request *request){
    char str[REQUEST_STR_SIZE]; /*request str:就是一个用\n分割的 大字符串*/
    size_t str_index = 0;

    if(request->type == GET){
        str_index = set_request_line(str,request);
    } else if(request->type == POST){
        //TODO: support POST
    }
    str[str_index+1] = '\0';
    printf("%s\n",str);

    return NULL;
}

struct http_response * parse_http_response(char * response){
    return NULL;
}


