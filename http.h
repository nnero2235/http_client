//
// Created by NNERO on 15/12/6.
//

#ifndef HTTP_CLIENT_HTTP_H
#define HTTP_CLIENT_HTTP_H

#include <time.h>

#define HTTP_VERSION "HTTP/1.1"

#define PARAM_SPLIT '?'
#define PARAM_AND '&'
#define PARAM_EQUAL '='

#define OK 200
#define REDIRECT 302
#define FORWARD 304
#define NOT_FOUND 404
#define SERVER_ERROR 500

#define HTML 0
#define XML 1
#define TEXT 2
#define PDF 3
#define JSON 4
#define GZIP 5
#define JPEG 6
#define PNG 7
#define MPEG 8

#define CHINESE 0


extern char *mime_type[];

extern char *language[];

extern char *connection_state[];

enum REQUEST_TYPE{
    GET,
    POST,
    PUT
};

/*
 GET/sample.jsp HTTP/1.1
Date:Mon,6 Oct2003 13:13:31 GMT
Accept:image/gif.image/jpeg
Accept-Language:zh-cn
Connection:Keep-Alive
Host:localhost
User-Agent:Mozila/4.0(compatible;MSIE5.01;Window NT5.0)
Accept-Encoding:gzip,deflate

username=jinqiao&password=1234
*/

struct http_request{
    enum REQUEST_TYPE type;
    char *request_line;
    struct request_headers *headers;
    struct request_param *params; /*遍历链表即可获得 所有param*/
    char *entity; /*主要是POST请求参数 暂时不能传文件*/
};

/*
    HTTP/1.1 200 OK
    Server:Apache Tomcat/5.0.12
    Date:Mon,6Oct2003 13:13:33 GMT
    Content-Type:text/html
    Content-Length:112
 */

struct http_response{
    int state_code;
    char *server;
    char *date;
    char *content_type;
    char *content_length;
    char *entity; /*暂时只支持文本*/
};

/*该结构本身是个链表*/
struct request_param{
    char *name;
    char *value;
    struct request_param *next;
};

struct request_headers{
    char *date;
    char *accept;
    char *accept_language;
    char *connection_state;
    char *accept_encoding;
    char *user_agent;
    char *host;
};

char* get_current_time();//获取当前时间

int make_client_socket(char *host,int port);/*返回-1 表示错误 正确返回sock_id*/

char * make_http_request(struct http_request *request); /*将request 转为string*/

struct http_response * parse_http_response(char *response_str);/*将string response 转为结构*/

#endif //HTTP_CLIENT_HTTP_H
