//
// Created by nnero on 15/12/8.
//

#ifndef HTTP_CLIENT_CLIENT_H_H
#define HTTP_CLIENT_CLIENT_H_H

#include "http.h"

typedef struct http_request * Request; //封装request
typedef struct http_response * Response; //封装response

Request create_request(Request_type type,char *url);

Response new_call(Request request);

char * get_entity(Response response);

void add_param(struct http_request *request,char *name,char *value);

void add_header(struct http_request *request,char *name,char *value);

//以下3个方法考虑private
int get_client_socket(char *host,int port);/*返回-1 表示错误 正确返回sock_id*/

char * build_http_request(struct http_request *request); /*将request 转为string*/

struct http_response * parse_http_response(char *response);/*将string response 转为结构*/

#endif //HTTP_CLIENT_CLIENT_H_H
