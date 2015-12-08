//
// Author: NNERO 
//
// TIME: 15/12/6 下午11:15
//
// DESC: 测试用例
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

struct request_param * create_param(char *name,char *value){
    struct request_param *param = malloc(sizeof(struct request_param));
    param->name = name;
    param->value = value;
    return param;
}

struct request_headers * create_headers(){
    struct request_headers *headers = malloc(sizeof(struct request_headers));
    char *temp;

    temp = get_current_time();
    headers->date = malloc((strlen(temp)+9)* sizeof(char));
    strncpy(headers->date,"Date:",5);
    strncpy(&headers->date[5],temp,strlen(temp));
//    headers->date[5+strlen(temp)] = '0';
    strncpy(&headers->date[4+strlen(temp)]," GMT",4);

    temp = "Connection:Keep-Alive";
    headers->connection_state = malloc(strlen(temp)* sizeof(char));
    strncpy(headers->connection_state,temp,strlen(temp));

    temp = mime_type[TEXT];
    headers->accept = malloc((strlen(temp)+7)* sizeof(char));
    strncpy(headers->accept,"Accept:",7);
    strncpy(&headers->accept[7],temp,strlen(temp));

    temp = language[0];
    headers->accept_language = malloc((strlen(temp)+16)* sizeof(char));
    strncpy(headers->accept_language,"Accept-Language:",16);
    strncpy(&headers->accept_language[16],temp,strlen(temp));

    temp = "Host:localhost";
    headers->host = malloc(strlen(temp)* sizeof(char));
    strncpy(headers->host,temp,strlen(temp));

    temp = "User-Agent:Mozila/4.0(compatible;MSIE5.01;Window NT5.0)";
    headers->user_agent = malloc(strlen(temp)* sizeof(char));
    strncpy(headers->user_agent,temp,strlen(temp));

    temp = "Accept-Encoding:gzip";
    headers->accept_encoding = malloc(strlen(temp)* sizeof(char));
    strncpy(headers->accept_encoding,temp,strlen(temp));

    return headers;
}


void test_make_http_request(){
    struct http_request request;
    struct request_param *params;
    request.type = GET;
    request.request_line = "index.html";

    params = create_param("pname","NNERO");
    request.params = params;
    params->next = create_param("age","13");
    params->next->next = create_param("sex","boy");
    params->next->next->next = NULL;

    request.headers = create_headers();

    make_http_request(&request);
}

int main(){
    test_make_http_request();
    return 0;
}



