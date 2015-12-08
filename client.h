//
// Created by nnero on 15/12/8.
//

#ifndef HTTP_CLIENT_CLIENT_H_H
#define HTTP_CLIENT_CLIENT_H_H

void add_param(struct request_param *params,char *name,char *value);

void add_header(struct request_header *headers,char *name,char *value);

int get_client_socket(char *host,int port);/*返回-1 表示错误 正确返回sock_id*/

char * build_http_request(struct http_request *request); /*将request 转为string*/

struct http_response * parse_http_response(char *response_str);/*将string response 转为结构*/

#endif //HTTP_CLIENT_CLIENT_H_H
